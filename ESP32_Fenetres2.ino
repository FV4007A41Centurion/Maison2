#include "Particle.h"

#define shortName "Lmsn2f"  // 7 chars max + 1 pour le state

const BleUuid serviceUuid("6c639be0-f04a-4eb0-9cd9-9ef9042ef73c");
const BleUuid rxUuid("6c639be1-f04a-4eb0-9cd9-9ef9042ef73c");
const BleUuid txUuid("6c639be2-f04a-4eb0-9cd9-9ef9042ef73c");

BleCharacteristic txCharacteristic("tx",
    BleCharacteristicProperty::NOTIFY | BleCharacteristicProperty::READ,
    txUuid, serviceUuid);

BleCharacteristic rxCharacteristic("rx",
    BleCharacteristicProperty::WRITE_WO_RSP,
    rxUuid, serviceUuid, onDataRecieved, NULL);

const uint8_t WINDOW_PIN1 = D2;
const uint8_t WINDOW_PIN2 = D4;
const uint8_t LED_PIN = D7;

bool window1Value = false;
bool window2Value = false;

unsigned long lastSend = 0;       // anti-spam timer
const uint16_t SEND_INTERVAL = 500; // ms


// ------------------- RX CALLBACK -------------------
void onDataRecieved(const uint8_t* data, size_t len,
                    const BlePeerDevice& peer, void* context)
{
    if (len == 0) return;

    if (data[0] == 1 || data[0] == '1') {
        digitalWrite(LED_PIN, HIGH);
    } else if (data[0] == 0 || data[0] == '0') {
        digitalWrite(LED_PIN, LOW);
    }
}


// ------------------- SETUP -------------------
void setup() {
Serial.begin (9600); 
    // Variables visibles dans la console Particle
    Particle.variable("window1Closed", window1Value);
    Particle.variable("window2Closed", window2Value);

    pinMode(LED_PIN, OUTPUT);
    pinMode(WINDOW_PIN1, INPUT_PULLDOWN);
    pinMode(WINDOW_PIN2, INPUT_PULLDOWN);

    // BLE init
    BLE.on();
    BLE.addCharacteristic(txCharacteristic);
    BLE.addCharacteristic(rxCharacteristic);

    advertiseValue("0"); // nom initial
}


// ------------------- MAIN LOOP -------------------
void loop() {

    // Lecture capteurs
    bool w1 = digitalRead(WINDOW_PIN1);
    bool w2 = digitalRead(WINDOW_PIN2);

    // Mise à jour variables cloud
    window1Value = w1;
    window2Value = w2;

    // Envoi BLE toutes les 500 ms
    if ( millis() - lastSend >= SEND_INTERVAL) {

        uint8_t buf[2];
        buf[0] = w1;
        buf[1] = w2;
        
  Serial.print("w1:");
  Serial.println(w1);
  Serial.print("w2:");
  Serial.println(w2);

        txCharacteristic.setValue(buf, 2);

        // Mettre l'état dans la pub (ex: Lmsn2fn01)
        String state = String(w1) + String(w2);
        advertiseValue(state);

        lastSend = millis();
    }
}


// ------------------- ADVERTISING -------------------
void advertiseValue(String val) {

    BleAdvertisingData data;
    data.clear();
    data.appendServiceUUID(serviceUuid);
    data.appendLocalName((String)shortName + val); // 8 chars max

    BLE.advertise(&data);
}
