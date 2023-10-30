/* Client Sketch by Rithvik Doshi

Trialling a client ESP32 to make sure we can dial in the logic of connecting client to server quickly and autonomously (without user input)

1. Scan for any advertising packets
2. If advertising found, attempt to connect with server
3. When connected, attempt handshake
  a. If handshake fails or times out, disconnect
4. Send over request to server, receive server output
5. Print out details
6. Then disconnect and wait 1 minute before searching again.

*/

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
