#include <ArduinoBLE.h>
#include <stdint.h>
#include <ArduinoMqttClient.h>
#include <WiFiS3.h>

// variables for button
const int alarmPin = 2;
long int alarmState = LOW; 

// Wifi
WiFiClient wifiClient;
const char ssid[] = "GG_TEP";  // your network SSID (name)
const char pass[] = "lolaflorez&";  // your network password (use for WPA, or use as key for WEP)

// MQTT (si la connection fail, regen adafruit key et update code ou reset le esp physiquement)
MqttClient mqttClient(wifiClient);
const char broker[] = "io.adafruit.com";
const int  port     = 1883;
const char publishTopic[] = "Alary/feeds/lamsn2out"; // Adafruit URL à partir du username
const char subscribeTopic[] = "Alary/feeds/lamsn2in";
const char topic[] = "Alary/feeds/lamsn2out";
const char username[] = "Alary"; // Adafruit
const char adafruitActiveKey[] = "key"; // Adafruit Active Key (bouton clef jaune)
const char clientId[] = "AlaryMonEsp32S3"; // Doit être unique

// Delay
const long interval = 10000;
unsigned long previousMillis = 0; // MQTT interval

unsigned long alarmMillisPrev = 0; // pour blink



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
  Serial.println("clientId set");
  mqttClient.setUsernamePassword(username, adafruitActiveKey);
  Serial.println("usename-password set");
  while (!mqttClient.connect(broker, port)) {
  Serial.print("MQTT connection failed! Error code = ");
  Serial.println(mqttClient.connectError());
  Serial.println("Retrying...");
  delay(1000);
  }
  Serial.println("You're connected to the MQTT broker!");

  // MQTT Subscribe
  mqttClient.subscribe(subscribeTopic);
}


void setup() {
  Serial.begin(9600);
  pinMode(alarmPin, OUTPUT);
  mqttSetup();
  //BLE init et scan
  while (!BLE.begin()) {
    Serial.println("Starting Bluetooth® Low Energy module failed!");
    BLE.end();
    Serial.println("Retrying (you should try do disconnect and reconnect the MCU)...");
    delay(10000);
  }
 Serial.println("BLE success!");

}



void loop() {
   BLE.scanForName("Lmsn2Pd1");
    BLEDevice doorperipheral1 = BLE.available();
  delay(1);
   BLE.scanForName("Lmsn2Pd0");
    BLEDevice doorperipheral0 = BLE.available();
  delay(5);
   BLE.scanForName("Lmsn2f00");
    BLEDevice windowperipheral00 = BLE.available();
  delay(1);
   BLE.scanForName("Lmsn2f10");
    BLEDevice windowperipheral10 = BLE.available();
  delay(1);
   BLE.scanForName("Lmsn2f01");
    BLEDevice windowperipheral01 = BLE.available();
  delay(1);
   BLE.scanForName("Lmsn2f11");
    BLEDevice windowperipheral11 = BLE.available();

 bool doorclosed = false;
 bool window1closed = false;
 bool window2closed = false;
 bool doorFound = true; // empêcher la corruption
 bool windowFound = true; // empêcher la corruption


  if (doorperipheral1) {
    Serial.print("Trouvé: ");
    Serial.println(doorperipheral1.localName());
    //Stop scanning. Connect and communicate.
    doorclosed = true;
  }
  else if (doorperipheral0) {
    Serial.print("Trouvé: ");
    Serial.println(doorperipheral0.localName());
    //Stop scanning. Connect and communicate.
    doorclosed = false;
  } else {
    doorFound = false;
  }
  if (windowperipheral00) {
    Serial.print("Trouvé: ");
    Serial.println(windowperipheral00.localName());
    //Stop scanning. Connect and communicate.
    window1closed = false;
    window2closed = false;
  } 
  else if (windowperipheral01) {
    Serial.print("Trouvé: ");
    Serial.println(windowperipheral01.localName());
    //Stop scanning. Connect and communicate.
    window1closed = true;
    window2closed = false;
  } 
  else if (windowperipheral10) {
    Serial.print("Trouvé: ");
    Serial.println(windowperipheral10.localName());
    //Stop scanning. Connect and communicate.
    window1closed = false;
    window2closed = true;
  } 
  else if (windowperipheral11) {
    Serial.print("Trouvé: ");
    Serial.println(windowperipheral11.localName());
    //Stop scanning. Connect and communicate.
    window1closed = true;
    window2closed = true;
  } else {
    windowFound = false;
  }
  BLE.stopScan();
  Serial.println("Done with BLE scan.");
 // call poll() regularly to allow the library to send MQTT keep alives which avoids being disconnected by the broker
  //  mqttClient.poll();

  // To avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay. See: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();
  //if (currentMillis - previousMillis >= interval) {
  if (true) {
    previousMillis = currentMillis;
    if (doorFound || windowFound) {
      Serial.println("Sending data to MQTT...");
    }
    // MQTT publish (envoyer)
    if (doorFound) {
      mqttClient.beginMessage(publishTopic);
      mqttClient.print("porte:");
      mqttClient.print((String)doorclosed);
      Serial.println("Door data sent to MQTT!");
      mqttClient.endMessage();
    }
    if (windowFound) {
      mqttClient.beginMessage(publishTopic);
      mqttClient.print("fenetre1:");
      mqttClient.print((String)window1closed);
      mqttClient.endMessage();

      mqttClient.beginMessage(publishTopic);
      mqttClient.print("fenetre2:");
      mqttClient.print((String)window2closed);
      Serial.println("Window data sent to MQTT!");
      mqttClient.endMessage();
    }
  }

  // MQTT subscribed (recevoir)
  if (mqttClient.parseMessage() > 0) {
    String newCode;
    bool isAlarme = false;
    while (mqttClient.available()) {
        // mettre dans une variable pour ne pas appeler read encore
        if (newCode == "alarme:") {
          isAlarme = true;
          newCode = "";
        }
        char read = (char)mqttClient.read();
        if (isdigit(read)) {
          newCode += read; // ajouter le chiffre
        }
    }
    if (isAlarme) {
      unsigned long alarmMillis = millis();
      if (newCode = "Normal"){
        Serial.println("Alarm off");
        digitalWrite(alarmPin, LOW);
      } if (newCode = "Relax"){
        Serial.println("Welcome home!");
        digitalWrite(alarmPin, HIGH);
      } if (newCode = "Alert"){
        Serial.println("Alarm on");
        if (alarmMillis >= alarmMillisPrev + 500) {
          alarmMillisPrev = alarmMillis;
          digitalWrite(alarmPin, !digitalRead(alarmPin)); // clignoter
        }
      }
    }
  }
}


// Callback for when BLE receives data
void onBLERx(BLEDevice peripheral, BLECharacteristic characteristic) {
  uint8_t buttonState[] = {0};
  characteristic.readValue(alarmState);
  buttonState[0] += 48; //convertir décimal en ascii
  Serial.print("État de l'alarme: ");
  Serial.write(alarmState);
  Serial.println(".");
}

void Communique(BLEDevice peripheral) {
  //Connect et trouve les charactéristiques DEL et Bouton

  //peripheral.discoverAttributes();
  BLECharacteristic ledCharacteristic = peripheral.characteristic("19B10011-E8F2-537E-4F6C-D104768A1214");
  BLECharacteristic buttonCharacteristic = peripheral.characteristic("19B10012-E8F2-537E-4F6C-D104768A1214");

  //Le peripheral va faire appeler la fonction onBLERx quand le bouton sera pesé
  buttonCharacteristic.setEventHandler(BLEWritten, onBLERx);
  buttonCharacteristic.subscribe();

  // Track previous LED state (to avoid redundant BLE writes)
  int oldalarmState = LOW;

  while (peripheral.connected()) {
    // Read both window sensors
    int alarmPin = digitalRead(alarmPin);

    // OR logic: if either is HIGH, LED should be ON
    

    if (alarmState != oldalarmState) {
      oldalarmState = alarmState;

      if (alarmState == HIGH) {
        Serial.println("One or both windows open, ALARM ON");
        ledCharacteristic.writeValue((byte)0x01);
      } else {
        Serial.println("Alarm off");
        ledCharacteristic.writeValue((byte)0x00);
      }
    }

    delay(50); // small debounce/loop delay
  }

 // Serial.println("Peripheral disconnected");
}
