/*
DetectDoorState Application on particle.io
*/

#include "Particle.h"
#define shortName "Lmsn2Pd" // 7 chars to set state as int on last char

const BleUuid serviceUuid("6c639be0-f04a-4eb0-9cd9-9ef9042ef73c");
const BleUuid rxUuid("6c639be1-f04a-4eb0-9cd9-9ef9042ef73c");
const BleUuid txUuid("6c639be2-f04a-4eb0-9cd9-9ef9042ef73c");

BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY|BleCharacteristicProperty::READ, txUuid, serviceUuid);
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataRecieved, NULL);
const uint8_t DOOR_PIN = D2;
const uint8_t LED_PIN = D7;
bool doorValue;
unsigned long lastSend = 0;       // anti-spam timer
const uint16_t SEND_INTERVAL = 250; // ms


// inutile?
void onDataRecieved(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
        // Allumer la DEL si le int/char 1 est reçu
        if(data[0]==1 || data[0]=='1'){
            digitalWrite(LED_PIN, HIGH);
        }
        // Éteindre la DEL si le int/char 0 est reçu
        if (data[0]==0 || data[0]=='0'){
            digitalWrite(LED_PIN, LOW);
        }
}

void setup() {
    Particle.variable("Door closed", &doorValue, BOOLEAN); // Peut être monitoré sur console

    pinMode(LED_PIN, OUTPUT);
    pinMode(DOOR_PIN, INPUT_PULLDOWN);
    
    // init BLE
    BLE.on();
    BLE.addCharacteristic(txCharacteristic);
    BLE.addCharacteristic(rxCharacteristic);
    advertiseValue("0");
}

// Note: Code that blocks for too long (like more than 5 seconds), can make weird things happen (like dropping the network connection).  The built-in delay function shown below safely interleaves required background activity, so arbitrarily long delays can safely be done if you need them.
void loop() {
    // la partie importante
    doorValue = digitalRead(DOOR_PIN); // Si l'état de la porte change
    if (millis() - lastSend >= SEND_INTERVAL) {
        // Evoyer statut de la porte
        uint8_t txBuf[] = {doorValue};
        txCharacteristic.setValue(txBuf, 1);
        advertiseValue((String)doorValue);
        // Attendre pour pas spam
        lastSend = millis();
    }
}

void advertiseValue(String val) {
    BleAdvertisingData data;
    data.clear();
    data.appendServiceUUID(serviceUuid);
    data.appendLocalName(shortName + val); // name must not be bigger than 8 chars
    BLE.advertise(&data);
}
