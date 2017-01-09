#include <RF24G.h>

RF24_G test;
void setup() {

  Serial.begin(9600);
  //Serial.println("what");\
  randomSeed(analogRead(0));
  test = RF24_G(4, 7, 8);
  //Serial.println("what");
  
}

void loop() {
  Serial.println("nice");
  int randNumber = random(300);
  int actual;
  int& recNumber = actual;
  
  

  packet sender;
  
  sender.setAddress(1);
  sender.setCnt(3);
  sender.addPayload(&randNumber, sizeof(int));
  Serial.println(randNumber);
  //Serial.println(sizeof(randNumber));
  //Serial.println(sender.getCnt());
  //Serial.println(sender.getAddress());
  sender.readPayload(&actual, sizeof(int));
  Serial.println(recNumber);
  Serial.println(test.write(sender));
  test.read(&sender);
  /*sender.address = 1;
  sender.bufferT =  &randNumber;

  packet reciver;
  reciver.bufferT = &recNumber;

  test.write(sender);

  test.read(&reciver);*/
  
  //delay();

}
