
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

void loop() {
  // create a random number
  uint8_t randNumber = random(300);
  // create a variable to store the received number
  int actual;
  // declare the sender packet variable
  packet sender;
  // declare the receiver packet variable
  packet receiver;
  // set the destination of the packet to address 1
  sender.setAddress(1);
  // write the payload to the packet
  sender.addPayload(&randNumber, sizeof(int));
  // print out the original payload
  Serial.print("original number:");
  Serial.println(randNumber);
  // send the packet, if it is successful try to read back the packet
  if (test.write(&sender) == true) {
    // wait until a packet is received
    while (test.available() != true);
    // copy the packet into the receiver object
    test.read(&receiver);
    // copy the payload into the actual value
    receiver.readPayload(&actual, sizeof(int));
    // print out the actual value received
    Serial.print("received number:");
    Serial.println(actual);
    
  }

}

