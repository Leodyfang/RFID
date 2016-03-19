
#include <Business.h>

#include <SPI.h>

#include <Option.h>



OPTION option;
oren o;

void setup(){
  Serial.begin(9600);
  SPI.begin();
}
void loop(){
  o = option.getOpt();
  
  switch(o){
    case up: BUSINESS::query(); break;
    
    case down: BUSINESS::deposit(); break;
    
    case left: 
      if(BUSINESS::registration()) {
        Serial.println("Registration SUCCESS!");
        BUSINESS::query();
      }
      else Serial.println("Registration FAILED, make sure it's a NEW card.");
      break;
    
    case right: 
      if(BUSINESS::destory()) {
        Serial.println("Destory SUCCESS!");
      }
      else Serial.println("Destory FAILED, no record in DataBase.");
      break;
  }
  
  delay(500);
  
}
