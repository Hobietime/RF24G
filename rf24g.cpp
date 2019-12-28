


#include <RF24.h>
#include "rf24g.h"



void packet::setAddress(uint8_t _address) {
	address = _address;
}

uint8_t packet::getAddress() const {
	return address;
}

uint8_t packet::getCnt() const {
	return cnt;
}

void packet::setCnt(uint8_t _cnt) {
	cnt = _cnt;
}

bool packet::addPayload(const void * data, const uint8_t size){
	if (size > 30) {
		return false;
	}
	const uint8_t* current = reinterpret_cast<const uint8_t*>(data);
	int i;
	for (i = 0; i < size; i++) {
		buffer[i] = *current++;
	}
	return true;
}

bool packet::readPayload(void * data, const uint8_t size) {
	if (size > 30) {
		return false;
	}
	uint8_t* current = reinterpret_cast<uint8_t*>(data);
	int i;
	for (i = 0; i < size; i++) {
		*current++ = buffer[i];
	}
	return true;
}

RF24_G::RF24_G(){
	radio = RF24(7,8);
	myAddress = 0;
}

RF24_G::RF24_G(uint8_t address, uint8_t _cepin, uint8_t _cspin) {
	int popIterator;
	for (popIterator = 0; popIterator < (MAX_NODES); popIterator++) {
		TXpacketCounters[popIterator] = 0;
		RXpacketCounters[popIterator] = PACKET_CNTER+1;
	}
	radio = RF24(_cepin,_cspin);
	myAddress = address;

	radio.begin();
  	radio.setChannel(108);
	radio.setAutoAck(1);                    // Ensure autoACK is enabled
	radio.setRetries(5 + myAddress,15);                 // Smallest time between retries, max no. of retries
	int pipenum = 1;
	for (popIterator = 0; popIterator < (MAX_NODES); popIterator++) {
		if ((popIterator) != myAddress) {
			uint64_t rxAddress = (BASE_ADDRESS + ((popIterator) + ((MAX_NODES + 1)  *  ( myAddress))));
			radio.openReadingPipe(pipenum, rxAddress);
			Serial.print("reading from to this address: ");
			Serial.println((int)(((popIterator) + ((MAX_NODES + 1)  *  ( myAddress)))));
			pipenum++;
		}
	}

	radio.setPALevel(RF24_PA_MAX);          // Set the power level to maximum
	radio.startListening();                 // Start listening

}

bool RF24_G::available() {
	return radio.available();
}


bool RF24_G::write(const packet* _packet) {
	radio.stopListening();
	bool success;
	int dest = _packet->getAddress();
	if (dest == myAddress){
		return false;
	}
	packet TX = *_packet;
	TX.setAddress(myAddress);
	TX.setCnt(TXpacketCounters[dest]);
	radio.openWritingPipe((BASE_ADDRESS + ((myAddress) + ((MAX_NODES + 1)  *  dest))));
	Serial.print("writing to this address: ");
	Serial.println((int)(((myAddress) + ((MAX_NODES + 1)  *  dest))));
	success = radio.write(&TX, 32);
	if (success == false) {
		radio.startListening();
		delayMicroseconds(TIMEOUT*myAddress);
		radio.stopListening();
		success = radio.write(&TX, 32);
	}
	if (success == true) {
		TXpacketCounters[dest] = ((TXpacketCounters[dest] + 1) & (PACKET_CNTER - 1) );
	}
	radio.startListening();
	return success;
}

bool RF24_G::read(packet* _packet) {
	packet receive;
	radio.stopListening();
	while (radio.available()) {
		radio.read(&receive, 32);
		if(receive.getCnt() != RXpacketCounters[receive.getAddress()]){
			*_packet = receive;
			RXpacketCounters[receive.getAddress()] = ((RXpacketCounters[receive.getAddress()] + 1) & (PACKET_CNTER - 1) );
			radio.startListening();
			return true;
		}
	}
	radio.startListening();
	return false;
}


bool RF24_G::setChannel(uint8_t channel) {
	if (channel > 125) {
		return false;
	}
	radio.stopListening();
	radio.setChannel(channel);
	radio.startListening();
	return true;
}