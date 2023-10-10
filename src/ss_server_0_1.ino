/*

This is a simple BLEServer Application
Author: Rithvik Doshi
Date: Oct 9 2023

*/

#include <ArduinoBLE.h>

int message = 0x420F;

BLEService safeStepsService("6969");

BLEUnsignedCharCharacteristic safeStepsChar("AFAF", BLERead | BLENotify);

long previousMillis = 0;  // last time the battery level was checked, in ms

void setup() {

  Serial.begin(9600); // Initialize Serial Communication
  while (!Serial);

  if (!BLE.begin()) { //Initialize BLE
    Serial.println("starting BLE failed!");

    while (1);
  }

  BLE.setLocalName("SafeSteps Beacon");
  BLE.setAdvertisedService(safeStepsService);
  safeStepsService.addCharacteristic(safeStepsChar);
  BLE.addService(safeStepsService);
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  BLE.advertise();

}

void loop() {

  // This is for one central device connection

  BLEDevice central = BLE.central();

  if (central) {
    // Serial.print("Connected to central: ");
    // // print the central's BT address:
    // Serial.println(central.address());

    previousMillis = millis();

    while (central.connected()) {
      long currentMillis = millis();
      // if 1s has passed, check the RSSI level:
      if (currentMillis - previousMillis >= 1000) {
        Serial.println("Checking RSSI...");
        Serial.println(central.rssi());
        while(currentMillis - previousMillis < 1500){
          currentMillis = millis();
        }
        // Serial.print("Disconnecting ");
        // Serial.print(central.address());
        // Serial.println("...");
        central.disconnect();
        while (central.connected());
        previousMillis = currentMillis;
        // Serial.print("Disconnected! Waiting for connection...");
      }

    }

      Serial.println("Loop exited");

  }

}

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}