
/* This sketch sends a packet with random data to another radio and waits for 
 * the packet to be sent back.  It prints out the random data and the received data, which should be the same.
 */

#include <rf24g.h>
// We must instantiate the RF24_G object outside of the setup function so it is available in the loop function
RF24_G test;
void setup() {
  Serial.begin(9600);
  // create the RF24G object with an address of 4, using pins 7 and 8
  test = RF24_G(4, 7, 8);

  
}

uint32_t timer = millis();

void loop() {
  // create a random number
  uint16_t randNumber = random(65535);
  // Wait 1 second between sending
  if (millis() - timer > 1000) {
    timer = millis();
    // declare the sender packet variable
    packet sender;
    // set the destination of the packet to address 1
    sender.setAddress(1);
    // write the payload to the packet
    sender.addPayload(&randNumber, sizeof(randNumber));
    // print out the original payload
    Serial.print("original number:");
    Serial.println(randNumber);
    // send the packet, if it is successful try to read back the packet
    bool ok = test.write(&sender);  // == true) {
                                    // Send data then check for a received packet
    if (ok) {
      Serial.println("Send OK");
    } else {
      Serial.println("Send Failed");
    }
  }

  // The NRF52 devices are software driven, so instead of using delay()
  // we need to constantly poll for available packets
  // NRF24 will receive data and send ACK packets internally (hardware driven)
  if (test.available() == true) {
    // declare the receiver packet variable
    packet receiver;
    // create a variable to store the received number
    uint16_t actual;
    // copy the packet into the receiver object
    test.read(&receiver);
    // copy the payload into the actual value
    receiver.readPayload(&actual, sizeof(randNumber));
    // print out the actual value received
    Serial.print("received number:");
    Serial.println(actual);

  }
}
