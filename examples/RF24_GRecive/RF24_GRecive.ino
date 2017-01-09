#include <RF24G.h>
#include "printf.h"
RF24_G test;
int i = 0;
void setup() {

  Serial.begin(9600);
  printf_begin();
  //Serial.println("what");
  test = RF24_G(1, 7, 8);
  randomSeed(analogRead(0));
  
  //Serial.println("what");
  test.radio.printDetails();
}

void loop() {
  
  packet sender;
 if (test.available()){
    test.read(&sender);
    Serial.print(sender.getCnt());
    test.write(sender);
 }
 

}
