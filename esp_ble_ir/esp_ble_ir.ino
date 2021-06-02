#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t kIrLed = 4;

#define SERVICE_UUID              "fb1e4001-54ae-4a28-9f74-dfccb248601d"
#define CHARACTERISTIC_UUID_RX    "fb1e4002-54ae-4a28-9f74-dfccb248601d"
#define CHARACTERISTIC_UUID_TX    "fb1e4003-54ae-4a28-9f74-dfccb248601d"

static BLECharacteristic* pCharacteristicTX;
static BLECharacteristic* pCharacteristicRX;

IRsend irsend(kIrLed);

static int irProtocol = 0;
static bool deviceConnected = false, sendCode = false;
static unsigned long irCode = 0x00UL;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;

    }
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


class MyCallbacks: public BLECharacteristicCallbacks {

    void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) {
      Serial.print("Status ");
      Serial.print(s);
      Serial.print(" on characteristic ");
      Serial.print(pCharacteristic->getUUID().toString().c_str());
      Serial.print(" with code ");
      Serial.println(code);
    }

    void onNotify(BLECharacteristic *pCharacteristic) {
      uint8_t* pData;
      std::string value = pCharacteristic->getValue();
      int len = value.length();
      pData = pCharacteristic->getData();
      if (pData != NULL) {
        Serial.print("Notify callback for characteristic ");
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(" of data length ");
        Serial.println(len);
        Serial.print("TX  ");
        for (int i = 0; i < len; i++) {
          Serial.printf("%02X ", pData[i]);
        }
        Serial.println();
      }
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
      uint8_t* pData;
      std::string value = pCharacteristic->getValue();
      int len = value.length();
      pData = pCharacteristic->getData();
      if (pData != NULL) {
        if (pData[0] == 0x1B) {
          irProtocol = pData[1];
          irCode = pData[2] * 256 * 256 * 256  + pData[3] * 256 * 256 + pData[4] * 256 + pData[5];
          sendCode = true;
        }
      }
    }


};

void initBLE() {
  BLEDevice::init("ESP IR BLE");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristicTX = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY );
  pCharacteristicRX = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  pCharacteristicRX->setCallbacks(new MyCallbacks());
  pCharacteristicTX->setCallbacks(new MyCallbacks());
  pCharacteristicTX->addDescriptor(new BLE2902());
  pCharacteristicTX->setNotifyProperty(true);
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); 
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}


void setup() {
  irsend.begin();
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  initBLE();

}

void loop() {
  if (sendCode) {
    switch (irProtocol) {
      case 1:
        irsend.sendNEC(irCode);
        break;
      case 2:
        irsend.sendSony(irCode, 12, 2);  // 12 bits & 2 repeats
        break;
    }
    sendCode = false;
  }

}
