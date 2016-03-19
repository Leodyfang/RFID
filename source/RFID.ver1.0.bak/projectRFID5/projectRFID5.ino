
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
    BUSINESS::query();
    
  } 
  else if(o == left){
    BUSINESS::deposit();
    
    
  }
  else if(o == right){
    BUSINESS::consume();
  }
  else if (o == down) {

  }
  delay(500);
  
}
