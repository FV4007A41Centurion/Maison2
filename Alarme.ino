#include <ArduinoMqttClient.h>
#include <WiFiS3.h>

#include <ArduinoBLE.h>

enum AlarmState {
  Normal = 0,
  Relax = 1,
  Alert = 2
};

const int ledPin = LED_BUILTIN; // mettre ledPin a LED du Arduino
int state = AlarmState::Normal;
String keypadCode = "123456";
bool doorOpen = false; // derrnière valeur connue
bool windowOpen = false; // derrnière valeur connue

// BLE
BLEService alarmService("19B10010-E8F2-537E-4F6C-D104768A1214"); // créer service(UUID)

// create switch characteristic and allow remote device to read and write
BLEByteCharacteristic alarmStateCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
// create button characteristic and allow remote device to get notifications
BLEByteCharacteristic windowCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic doorCharacteristic("19B10013-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

// Wifi
WiFiClient wifiClient;
const char ssid[] = "GG_TEP";  // your network SSID (name)
const char pass[] = "lolaflorez&";  // your network password (use for WPA, or use as key for WEP)

// MQTT (si la connection fail, regen adafruit key et update code ou reset le esp physiquement)
MqttClient mqttClient(wifiClient);
const char broker[] = "io.adafruit.com";
const int        port     = 1883;
const char publishTopic[] = "Alary/feeds/lamsn2out"; // Adafruit URL à partir du username
const char subscribeTopic[] = "Alary/feeds/lamsn2in";
const char topic[] = "Alary/feeds/lamsn2out";
const char username[] = "Alary"; // Adafruit
const char adafruitActiveKey[] = "aio_RgfH67GvYbTWTFYm5ETDFde6RtTL"; // Adafruit Active Key (bouton clef jaune)
const char clientId[] = "AlaryMonEsp32-S3"; // Doit être unique

// Delay
const long interval = 10000;
unsigned long previousMillis = 0;
int count = 0;

unsigned long alarmMillisPrev = 0;


void bleSetup() {
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set the local name peripheral advertises
  BLE.setLocalName("lamaisonnetten2");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(alarmService);

  // add the characteristics to the service
  alarmService.addCharacteristic(alarmStateCharacteristic);
  alarmService.addCharacteristic(windowCharacteristic);
  alarmService.addCharacteristic(doorCharacteristic);

  // add the service
  BLE.addService(alarmService);

  alarmStateCharacteristic.writeValue(0);
  windowCharacteristic.writeValue(0);
  doorCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");
}

void mqttSetup() {
  // Wifi connect
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
  // failed, retry
  Serial.print(".");
  delay(5000);
  }
  Serial.println("You're connected to the wifi!");

  // MQTT connect
  mqttClient.setId(clientId);
  mqttClient.setUsernamePassword(username, adafruitActiveKey);
  if (!mqttClient.connect(broker, port)) {
  Serial.print("MQTT connection failed! Error code = ");
  Serial.println(mqttClient.connectError());
  while (1);
  }
  Serial.println("You're connected to the MQTT broker!");

  // MQTT Subscribe
  mqttClient.subscribe(subscribeTopic);
}




void setup(){
  Serial.begin(9600);
  while (!Serial);

  pinMode(ledPin, OUTPUT); // use the LED as an output
  mqttSetup();
  bleSetup();
}




void bleLoop() {
  // poll for Bluetooth® Low Energy events
  BLE.poll();

  bool stateChanged = (doorCharacteristic.value() != doorOpen || windowCharacteristic.value() != windowOpen);

  if (stateChanged) {
    // state changed, update characteristics
    if (doorCharacteristic.value() == 0x01) {
      alarmStateCharacteristic.writeValue(0x01); // DEL allumée constante
    } else if (windowCharacteristic.value() == 0x01) {
      alarmStateCharacteristic.writeValue(0x02); // DEL allumée clignotante
    } else {
      alarmStateCharacteristic.writeValue(0x00); // DEL éteinte
    }
    state = alarmStateCharacteristic.value();
  }
  doorOpen = doorCharacteristic.value();
  windowOpen = windowCharacteristic.value();
  unsigned long alarmMillis = millis();
  switch(state) {
      case AlarmState::Normal:
        if (stateChanged) {
          Serial.println("Alarm off");
          digitalWrite(ledPin, LOW);
        }
        break;
      case AlarmState::Relax:
        if (stateChanged) {
          Serial.println("Welcome home!");
          digitalWrite(ledPin, HIGH);
        }
        break;
      case AlarmState::Alert:
        if (stateChanged) {
          Serial.println("Alarm on");
        }
        if (alarmMillis >= alarmMillisPrev + 500) {
          alarmMillisPrev = alarmMillis;
          digitalWrite(ledPin, !digitalRead(ledPin)); // clignoter
        }
        break;
  }
}

void mqttLoop() {
  // call poll() regularly to allow the library to send MQTT keep alives which avoids being disconnected by the broker
  mqttClient.poll();

  // To avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay. See: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
  previousMillis = currentMillis;

  //MQTT publish (envoyer)
  mqttClient.beginMessage(publishTopic);
  mqttClient.print((String)state);
  //mqttClient.print(count++);
  mqttClient.endMessage();
  }

  //MQTT subscribed (recevoir)
  if (mqttClient.parseMessage() > 0) {
 String newCode = "";
    bool statuschange = false;
    while (mqttClient.available()) {
        if (newCode == "alarme:Alert"){
        statuschange = true;
        digitalWrite(LED_BUILTIN, HIGH);
        }
        // mettre dans une variable pour ne pas appeler read encore
        char read = (char)mqttClient.read();
        if (isdigit(read)) {
          
          newCode += read; // ajouter le chiffre
        }
    }
    Serial.print("Changed password from \"" + keypadCode);
    keypadCode = newCode;
    Serial.println("\" to \"" + keypadCode + "\"");
  }
}




void loop() {
  bleLoop();
  mqttLoop();
}
