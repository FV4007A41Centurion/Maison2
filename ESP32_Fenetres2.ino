#include <ArduinoBLE.h>

const int window1Pin = 2;
const int window2Pin = 3;

BLEService Service("78bcafe0-4f63-4222-a7ab-8bb05d7fd6cf"); // create service

// create button characteristic and allow remote device to get notifications
BLEByteCharacteristic windowCharacteristic("78bcafe1-4f63-4222-a7ab-8bb05d7fd6cf", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Program begin");

  pinMode(window1Pin, INPUT); // use button pin as an input
  pinMode(window2Pin, INPUT); // use button pin as an input

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  } else {
    Serial.println("BLE success");
  }

  // set the local name peripheral advertises
  BLE.setLocalName("lamaisonnetten2fenetre");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(Service);

  // add the characteristics to the service
  Service.addCharacteristic(windowCharacteristic);

  // add the service
  BLE.addService(Service);

  windowCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");
}

void loop() {
  // poll for Bluetooth® Low Energy events
  BLE.poll();

  // read the current button pin state
  char windowValue = digitalRead(window1Pin) || digitalRead(window2Pin);

  // has the value changed since the last read
  bool stateChanged = (windowCharacteristic.value() != windowValue);

  if (stateChanged) {
    // button state changed, update characteristics
    windowCharacteristic.writeValue(windowValue);
  }
}
