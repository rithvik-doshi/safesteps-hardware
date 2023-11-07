/*
    BLEServer that notifies the client device. Basically an attempt to replicate BLE_notify_example by Evandro Copercini using ArduinoBLE.h
    Rithvik Doshi

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

*/

#include <ArduinoBLE.h>

// bool deviceConnected = false;
// bool oldDeviceConnected = false;
int message = 0x420F;
int broadcast = 0x2468;
long last_time = 0, second_time = 0;
BLEService safeStepsService("6969");
BLEIntCharacteristic safeStepsCharacteristic("AFAF", BLERead | BLENotify | BLEWrite | BLEIndicate);
BLEIntCharacteristic broadcastCharacteristic("2468", BLERead | BLENotify | BLEBroadcast);
BLEDescriptor safeStepsDescriptor("420F", "message");

int LED_BUILTIN = 2;
int onoff = 0x0;

void setup() {
  Serial.begin(9600); // Initialize Serial Communication
  while (!Serial);

  pinMode (LED_BUILTIN, OUTPUT);

  if (!BLE.begin()) { //Initialize BLE
    Serial.println("starting BLE failed!");

    while (1);
  }

  BLE.setDeviceName("Arduino ESP32");
  BLE.setLocalName("SafeSteps Beacon");

  safeStepsCharacteristic.addDescriptor(safeStepsDescriptor);
  safeStepsService.addCharacteristic(safeStepsCharacteristic);
  safeStepsService.addCharacteristic(broadcastCharacteristic);

  BLE.setAdvertisedService(safeStepsService);
  BLE.addService(safeStepsService);

  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  BLE.advertise();

  Serial.println("Waiting on a client connection to notify...");

  // broadcastCharacteristic.writeValue(broadcast);
  // broadcastCharacteristic.broadcast();

}

void loop() {

  // Set the advertising interval in units of 0.625 ms. Defaults to 100ms (160 * 0.625 ms) if not provided.
  // 20ms = 0.625*32
  BLE.setAdvertisingInterval(32);
  BLEDevice central = look20ms();

  // 152.5ms = 244*0.6.25
  if (!central){
    BLE.setAdvertisingInterval(244);
    central = lookLonger();
  }

  if (central) {
    last_time = millis();
    while (central.connected()){
      
      central.poll(); // This doesn't really have an effect on things it seems
      if (millis() > last_time + 1000){
        // Serial.println(central.rssi());
        broadcastCharacteristic.writeValue(broadcast++);
        // broadcastCharacteristic.broadcast(); // What does this do tho??? 
        if (safeStepsCharacteristic.subscribed()){
          safeStepsCharacteristic.writeValue(message);
          Serial.print("Sent message: ");
          Serial.println(message, HEX);
          message++;
        }
        last_time = millis();
      }

      if (safeStepsCharacteristic.written()){
        if (safeStepsCharacteristic.value()){
          Serial.print("Value written: ");
          Serial.println(safeStepsCharacteristic.value());
        }
      }

    }
    Serial.println("Disconnected and message reset.");
    message = 0x420F;
    broadcast = 0x2468;
    delay(3);
  }
}

BLEDevice look20ms(){
  // https://community.cypress.com/t5/Knowledge-Base-Articles/Making-a-BLE-Device-Discoverable-on-iOS-Devices-KBA223312/ta-p/250101
  last_time = millis();
  second_time = millis();
  Serial.println("Start advertising @ 20ms...");
  while (millis() < last_time + 30000){
    if (millis() > second_time + 500){
      // broadcastCharacteristic.broadcast();
      second_time = millis();
      onoff = !onoff;
      digitalWrite(LED_BUILTIN, onoff);
    }
    BLEDevice central = BLE.central();
    if (central){
      return central;
    }
    // delay(20);
  }
  return BLE.central();
}

BLEDevice lookLonger(){
  Serial.println("Advertising at 152.5ms");
  second_time = millis();
  while (1){
    if (millis() > second_time + 1000){
      // broadcastCharacteristic.broadcast();
      second_time = millis();
      onoff = !onoff;
      digitalWrite(LED_BUILTIN, onoff);
    }
    BLEDevice central = BLE.central();
    if (central){
      return central;
    }
    // delay(152.5);
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