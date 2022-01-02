#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define BLE_NAME "I AM ESP32"

#define SERVICE_REGITSTRATION_UUID "459aa3b5-52c3-4d75-a64b-9cd76f65cfbb"
#define SERVICE_WORK_TIME_UUID "cafa0333-8a16-4a59-b706-2f0e3fd38f58"

#define CHARACTERISTIC_REGITSTRATION_CREDENTIALS_UUID "b9e70f80-d55e-4cd7-bec6-14be34590efc"
#define CHARACTERISTIC_REGITSTRATION_RESPONSE_UUID "7048479a-23f2-4f5b-8113-e60e59294b5a"

#define CHARACTERISTIC_WORK_TIME_UUID "2c1529cd-f45d-4739-9738-2886fe46f7f1"

#define PIN_LED 2

BLECharacteristic *pCharacteristicRegistrationCredentials;
BLECharacteristic *pCharacteristicRegistrationresponse;
BLECharacteristic *pCharacteristicWorkTime;

unsigned long timerMillis = 0;
bool isBleConnected = false;

void enableBluetooth();
void sendWorkedTime(int seconds);

class BLECharacteristicRegistrationResponse : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0)
    {
      Serial.println("*********");
      Serial.print("New value: ");
      for (int i = 0; i < value.length(); i++)
      {
        Serial.print(value[i]);
      }
      Serial.println();

      pCharacteristicRegistrationresponse->setValue("0");
      pCharacteristicRegistrationresponse->notify();
    }
  }
};

class BluetoothServerEventCallback : public BLEServerCallbacks
{
  void onConnect(BLEServer *server)
  {
    isBleConnected = true;
    // sendDeviceState();
    BLEDevice::stopAdvertising();
  }
  void onDisconnect(BLEServer *server)
  {
    isBleConnected = false;
    BLEDevice::startAdvertising();
  }
};

void setup()
{
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  enableBluetooth();
}

void loop()
{
  digitalWrite(PIN_LED, isBleConnected ? HIGH : LOW);

  if (millis() - timerMillis > 5000)
  {
    timerMillis = millis();

    sendWorkedTime(timerMillis / 1000);
  }
}

void enableBluetooth()
{
  Serial.println("Starting BLE work!");
  // BLEDevice::init(BLE_NAME + ESP.getEfuseMac());
  BLEDevice::init(BLE_NAME);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BluetoothServerEventCallback());

  BLEService *pServiceRegistration = pServer->createService(SERVICE_REGITSTRATION_UUID);
  pCharacteristicRegistrationCredentials = pServiceRegistration->createCharacteristic(
      CHARACTERISTIC_REGITSTRATION_CREDENTIALS_UUID, BLECharacteristic::PROPERTY_WRITE);
  pCharacteristicRegistrationresponse = pServiceRegistration->createCharacteristic(
      CHARACTERISTIC_REGITSTRATION_RESPONSE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristicRegistrationresponse->addDescriptor(new BLE2902());

  pCharacteristicRegistrationCredentials->setCallbacks(new BLECharacteristicRegistrationResponse());

  BLEService *pServiceWorkTime = pServer->createService(SERVICE_WORK_TIME_UUID);
  pCharacteristicWorkTime = pServiceWorkTime->createCharacteristic(
      CHARACTERISTIC_WORK_TIME_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristicWorkTime->addDescriptor(new BLE2902());

  pCharacteristicRegistrationCredentials->setValue("Tell me a few sweet words");

  pServiceRegistration->start();
  pServiceWorkTime->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_REGITSTRATION_UUID);
  pAdvertising->addServiceUUID(SERVICE_WORK_TIME_UUID);
  // pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void sendWorkedTime(int seconds)
{
  if (isBleConnected)
  {
    pCharacteristicWorkTime->setValue(seconds);
    pCharacteristicWorkTime->notify();
  }
}
