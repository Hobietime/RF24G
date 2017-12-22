
/**
 * @file rf24g.h
 * @author Caio Motta
 * @date 19 Sep 2016
 * @brief A simple interface for the RF24 radio that abstracts thmr20's Driver.
 *
 * This library provides a simple way for up to 6 nRF24L01 radios to communicate with each other.
 *
 * @see http://tmrh20.github.io/RF24/
 * @see https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
 */


#ifndef __RF24G_H__
#define __RF24G_H__

#include "RF24.h"

#define PACKET_CNTER 32

#define MAX_NODES 6

#define BASE_ADDRESS 0xDEADBEEF00LL


#define TIMEOUT 5

class packet {
	/**
   * @name Packet object that is sent via the radios
   *
   *  This is the object that is passed to the radio to be sent.  It has the source/destination address, a counter, and the payload inside.
   */
  /**@{*/
private:
	uint8_t address;
	uint8_t cnt;
	byte buffer[30];
public:
/**
   * @name Packet public interface
   *
   *  These are the main methods you need to set, modify, and retrieve data from packets.
   */
  /**@{*/


	/**
	   * Sets the address of a packet.  
	   *
	   * If you are sending a packet, set this to set the destination of the packet.
	   * 
	   */
	void setAddress(uint8_t _address);

	/**
	   * Gets the address of a packet.  
	   *
	   * If you receive a packet, call this on the packet to get what address the packet came from.
	   *
	   * @return Current packet address.
	   */
	uint8_t getAddress() const;

	/**
	   * Gets the counter of a packet.  
	   *
	   * This is used internally by the library to set the packet counter.  This is used to detect duplicate packets.
	   *
	   * The user does not need to use this method.
	   *
	   * @return Current packet counter.
	   */
	uint8_t getCnt() const;

	/**
	   * Sets the counter of a packet.  
	   *
	   * This is used internally by the library to set the packet counter.  This is used to detect duplicate packets.
	   *
	   * The user does not need to use this method.
	   *
	   */
	void setCnt(uint8_t _cnt);

	/**
	   * Adds any datatype smaller than 30 bytes to the packet. 
	   * @note There is no way to determine what kind of datatype is in this packet without prior knowledge.
	   * @note If you want to send different types of payloads, use a struct or class similar to this packet within the payload that contains metadata on what type of data it is.
	   *
	   * This needs the address of an object and it's size to work correctly.
	   * @code
	   *	//addPayload() example:
	   *
	   *			int var = 23;
	   *			if (packet.addPayload( &value, sizeof(var)) == false) {
	   *				Serial.println("Datatype is too large!")
	   *			}
	   * @endcode 
	   *
	   * @return True if the size is within 30 bytes,  false if it is not. 
	   *
	   * @warning This does not allow for you to overwrite the packet.  But it is possible to overread from locations in memory that are adjacent to an object!  Always use sizeof(yourObject) to prevent this.
	   */

	bool addPayload(const void * data, const uint8_t size);

	/**
	   * Retrieves any datatype smaller than 30 bytes from the packet. 
	   * @note There is no way to determine what kind of datatype is in this packet.
	   * @note If you want to send multiple values, use a struct or class similar to this packet within the payload.
	   *
	   * This needs the address of an object and it's size to work correctly.
	   *
	   * @code
	   *	//readPayload() example:
	   *
	   *			int var;
	   *			if (packet.readPayload( &var, sizeof(var)) == false) {
	   *				Serial.println("Datatype is too large!")
	   *			}
	   * @endcode 
	   *
	   * @note The variable \a var will have a new value from the packet.
	   *
	   * @return True if the size is within 30 bytes,  false if it is not.
	   *
	   * @warning If you specify a size that is larger than the object you wish to write to, you can write into adjacent memory!  
	   * @warning This \a probably will crash your program and/or give you junk data in other parts of your code!  Always use sizeof(yourObject) to prevent this.
	   */
	bool readPayload(void * data, const uint8_t size);
};




class RF24_G {
/** 
   * \example rf24g_send.cpp
   * \example rf24g_receive.cpp
   * This is an example on how to receive using the RF24_G class
   */
private:
	int myAddress;
	uint8_t TXpacketCounters[MAX_NODES];
	uint8_t RXpacketCounters[MAX_NODES];
	RF24 radio{8,9};
public:
/**
   * @name Primary public interface
   *  
   *  These are the main methods you need to send and receive data.
   */
  /**@{*/

	/**
	   * Default Constructor
	   *
	   * Creates a new instance of the radio object.  This configures tmrh20's driver to default settings.  
	   * Use this if you want to instantiate the radio class, but initialize it later.
	   */
	RF24_G();

	/**
	   * Constructor
	   *
	   * Creates a new instance of the radio object.  This configures tmrh20's driver.  Before using, you create an instance
	   * and send in the unique pins that this chip is connected to.
	   * If you have followed the wiring diagram on the first page, the CE pin should be 7 and the CS pin should be 8. 
	   *
	   * @param address The address of tis radio instance
	   * @param _cepin The pin attached to Chip Enable (CE) pin on the RF module
	   * @param _cspin The pin attached to Chip Select (CS) pin
	   */	
	RF24_G(uint8_t address, uint8_t _cepin, uint8_t _cspin);

	/**
	   * Checks if there is a packet received packet to be read
	   *
	   * 
	   * @return True if a packet is available, false if not. 
	   * 
	   */
	bool available();

	/**
	   * Writes data to a packet.
	   * The packet is passed by reference, this means we need to use the & operator.
	   *
	   * @code
	   *	//write() example:
	   *
	   *			int var;
	   *			if (radio.write( &packet) == false) {
	   *				Serial.println("Transmission failed!")
	   *			}
	   * @endcode 
	   * 
	   * @return True if a packet was sent successfully, false if not. 
	   * @note Just because a packet was not sent successfully, it does not mean a packet was not received by the target radio! 
	   * @note This could be due to the sender not receiving the confirmation that the target radio has received the packet.
	   * @note This could be fixed with a 3 way handshake, but that is not supported in hardware and would be slow in software.
	   * @note If you are afraid to send the same data twice, don't worry. Duplicate packets are taken care of at the reciving side.
	   */
	bool write(const packet* _packet);
	/**
	   * Reads a packet.
	   * The packet is passed by reference, this means we need to use the & operator.
	   *
	   * @code
	   *	//read() example:
	   *
	   *			int var;
	   *			if (radio.read( &packet) == false) {
	   *				Serial.println("Receive failed!")
	   *			}
	   * @endcode 
	   * 
	   * @return True if a packet was read successfully, false if not. 
	   * 
	   */
	bool read(packet* _packet);
	/**
	   * Sets the channel to use
	   * @note The available channels are 0-125, but channels 108+ are out of the wifi band and recommended.
	   * 
	   * @return True if the channel was set successfully, false if not. 
	   * 
	   */
	bool setChannel(uint8_t channel);

};

/**
 * @mainpage A simple interface for the RF24 radio that abstracts thmr20's driver.
 *
 * @section About About
 *
 * The nRF24L01+ wireless transceiver board allow for wireless communication between two or more radios at distances greater than Bluetooth or standard WiFi. This tutorial includes an overview of the different types or radios available in the store, wiring the radios to an Arduino, an example sketch that allows for two way communication, and finally tips and tricks to increase your success with the radios.
 *
 * @section Purchasing Purchasing
 *
 * There are two versions available in the UCSB ECE store. They also can be purchased on the yourduino website (http://www.yourduino.com/sunshop/) The high power transceiver has amplifiers and an external antenna.  It has been tested to work at ranges in excess of 350 meters. The low power transceivers have an internal antenna and work at about 20 meters.  The two different types can work together.  Se the tips and tricks section for more info. 
 * 
 * @section Installation Installation
 *
 * This library requires thmr20's radio driver.  Both can be found in the Arduino repository.
 *  
 * ## First, go to sketch→Include Library→Manage Libraries...
 * ![First, go to sketch→Include Library→Manage Libraries...](step1.png)
 * ## The library manager will show as an additional window.  
 * ## Search for rf24 and select version 1.1.7 of TMRh20’s RF24 Library.
 * ![The library manager will show as an additional window.  Search for rf24 and select version 1.1.7 of TMRh20’s RF24 Library.](step2.png)
 * ## Press install.  
 * ## Next, add version 1.0 of the RF24G library.
 * ![Next, add version 1.0 of the RF24G library.](step3.PNG)
 * ## Press install. 
 * # Wiring
 * ### This tutorial assumes you are using the RF24 modules sold here: http://yourduino.com/sunshop//index.php?l=product_detail&p=489
 * ### recouses for this tutorial are based on Terry's instructions at https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
 * ## First, attach the either the high power or low power radio module to the base module
 * ![LOW_POWER](LP_MODULE.jpg)
 * ![HIGH_POWER](HP_MODULE.jpg)
 * ## Next, connect jumpers between the Arduino and the base module using this table
 * ![table](table.png)
 * ### More in-depth instructions can be found at https://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
 * # General concepts
 * ## This library provides an abstraction layer that allows the user identify each radio by an address and each transmission as a packet.
 * ### Up to 6 radios can be used in the network, with each having a unique address: (0, 1, 2, 3, 4, 5).
 * ### Each radio is initialized using an #RF24_G object, which provides the ability to read and write packets.
 * ### This library uses the built in functions of the radio to ensure guaranteed delivery; However, like any practical guaranteed transmission network, there is a timeout.
 * ### The after 30 retransmit attempts, the radio gives up and returns that it has failed to transmit a packet.  More info can be seen it the #RF24_G::read() docs.
 * ### The #packet class is an object that contains all the necessary information to bring data to and from each radio, as well as let each radio keep track of any dropped packets.
 * ### A #packet allows for any playload that is 30 bytes long. The payload can be any type or array of types.
 * # How to use this documentation. 
 * ## Read the #packet and #RF24_G class documentation. It provides a description of what every class and object in the library is for.b  
 * ## Check the examples to understand the way the two classes are used to send data from one radio to another.
 */
 #endif