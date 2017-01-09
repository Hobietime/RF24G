#include <RF24G.h>

RF24_G test;
void setup() {
	Serial.begin(9600);
	// create the RF24G object with an address of 4, using pins 7 and 8
	test = RF24_G(4, 7, 8);
	// print out the details of the radio's configuration (useful for debug)
	test.radio.printDetails();
	
}

void loop() {
	// create a random number
	uint8_t randNumber = random(300);
	// create a variable to store the received number
	uint8_t actual;
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
	if (test.write(sender) == true) {
		// check if a packet is received
		if (test.available() == true) {
			// copy the packet into the receiver object
			test.read(&receiver);
			// copy the payload into the actual value
			receiver.readPayload(actual, sizeof(int));
			// print out the actual value received
			Serial.print("received number:");
			Serial.println(actual);
		}
	}

}
