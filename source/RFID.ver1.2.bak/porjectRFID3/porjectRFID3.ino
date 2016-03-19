
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
  if (o == up) {
    STORAGE::flushDataBase();
    
  } 
  else if(o == left){
    if(BUSINESS::registration())
      Serial.println("Success!");
    
  }
  else if(o == right){
    if(BUSINESS::destory())
      Serial.println("Success!");
  }
  else if (o == down) {
    for (int i = 0; i < 512; i++) { 
      Serial.print(EEPROM.read(i), DEC);
      Serial.println(); 
      delay(500);
    }
  }
  delay(500);
  
}
