/* Client Sketch by Rithvik Doshi

Trialling a client ESP32 to make sure we can dial in the logic of connecting client to server quickly and autonomously (without user input)

1. Scan for any advertising packets from "SafeSteps Beacon"
2. If advertising found, attempt to connect with server
3. When connected, attempt handshake
  a. If handshake fails or times out, disconnect
4. Send over request to server, receive server output
5. Print out details
6. Then disconnect and wait 1 minute before searching again.

*/


# include <ArduinoBLE.h>

void explorerPeripheral(BLEDevice peripheral);

int LED_BUILTIN = 2;
int onoff = 0x0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode (LED_BUILTIN, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  Serial.println("Bluetooth® Low Energy Central scan + connect. Waiting for SafeSteps Beacon...");

  // Start scanning for peripherals
  // Takes optional parameter: true/false to disable/enable filtering advertisements received more than once
  BLE.scan();

}

void loop() {
  onoff = !onoff;
  digitalWrite(LED_BUILTIN, onoff);
  delay(100);
  BLEDevice peripheral = BLE.available();

  if (peripheral) {

    // if (peripheral.hasService("6969")){
    //   Serial.println("Yup, this is the beacon!");
    // } else {
    //   // Serial.println("Hmm, this didn't get picked up");
    // }

    // for (int i = 0; i < peripheral.serviceCount(); i++) {
    //   BLEService service = peripheral.service(i);
    //   // exploreService(service);
    //   if (service.uuid() == "6969"){
    //     Serial.println("Got it! 6969 was detected.");
    //   }
    // }

    // if (){
    //   Serial.println("We got em!");
    // }

    if (peripheral.advertisedServiceUuid() == "6969"){
      BLE.stopScan();
      explorerPeripheral(peripheral);

      Serial.println("Explore done!");
      Serial.println("This program's done. You can reset now...");
      while (1){
        delay(1500);
        onoff = !onoff;
        digitalWrite(LED_BUILTIN, onoff);
      }
    }
  }

  // delay(3);

}

void explorerPeripheral(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  // read and print device name of peripheral
  Serial.println();
    // Print Local Name
  Serial.print("Local name: ");
  Serial.println(peripheral.localName());
  Serial.print("Device name: ");
  Serial.println(peripheral.deviceName());
  Serial.print("Appearance: 0x");
  Serial.println(peripheral.appearance(), HEX);
  Serial.println();

  // loop the services of the peripheral and explore each
  for (int i = 0; i < peripheral.serviceCount(); i++) {
    BLEService service = peripheral.service(i);

      if (service.uuid() == "6969"){
        Serial.println("Got it! 6969 was detected.");
      }

    exploreService(service);
  }

  Serial.println();

  // we are done exploring, disconnect
  Serial.println("Disconnecting ...");
  peripheral.disconnect();
  Serial.println("Disconnected");
}

void exploreService(BLEService service) {
  // print the UUID of the service
  Serial.print("Service ");
  Serial.println(service.uuid());

  // loop the characteristics of the service and explore each
  for (int i = 0; i < service.characteristicCount(); i++) {
    BLECharacteristic characteristic = service.characteristic(i);
    exploreCharacteristic(characteristic);
  }
}

void exploreCharacteristic(BLECharacteristic characteristic) {
  // print the UUID and properties of the characteristic
  Serial.print("\tCharacteristic ");
  Serial.print(characteristic.uuid());
  Serial.print(", properties 0x");
  Serial.print(characteristic.properties(), HEX);

  // check if the characteristic is readable
  if (characteristic.canRead()) {
    // read the characteristic value
    characteristic.read();

    if (characteristic.valueLength() > 0) {
      // print out the value of the characteristic
      Serial.print(", value 0x");
      printData(characteristic.value(), characteristic.valueLength());
    }
  }
  Serial.println();

  // loop the descriptors of the characteristic and explore each
  for (int i = 0; i < characteristic.descriptorCount(); i++) {
    BLEDescriptor descriptor = characteristic.descriptor(i);

    exploreDescriptor(descriptor);
  }
}

void exploreDescriptor(BLEDescriptor descriptor) {
  // print the UUID of the descriptor
  Serial.print("\t\tDescriptor ");
  Serial.print(descriptor.uuid());

  // read the descriptor value
  descriptor.read();

  // print out the value of the descriptor
  Serial.print(", value 0x");
  printData(descriptor.value(), descriptor.valueLength());

  Serial.println();
}

void printData(const unsigned char data[], int length) {
  for (int i = 0; i < length; i++) {
    unsigned char b = data[i];

    if (b < 16) {
      Serial.print("0");
    }

    Serial.print(b, HEX);
  }
}
