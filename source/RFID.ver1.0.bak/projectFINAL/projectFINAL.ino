/*
 * Main Function : Create UI
 * Creator: sketchpink
 * Time: 2015.10.1
 */
 
#include <UI.h>
#include <SPI.h>      //Already included in RFID.h
#include <Option.h>   //re-include?
 
OPTION option;        //system control
oren o;

void setup(){
  Serial.begin(9600);
  SPI.begin();
}

void loop(){
    
  Serial.println("=================WELCOME=================");
  Serial.println("Menu List: up    -- DataBase Check");
  Serial.println("           left  -- Management");
  Serial.println("           right -- Consumption");
  Serial.println("           down  -- Reboot");
  Serial.println("=========================================");
  Serial.println();
  Serial.flush();
   
  o = option.getOpt();
  switch(o){
    case up: UI::Database(); break;
    case left: UI::Management(); break;
    case right: UI::Consumption(); break;
    case down: 
      Serial.println("===========Restarting System...==========");
      Serial.println();
      break;
  }
    
  delay(500);
 
}


