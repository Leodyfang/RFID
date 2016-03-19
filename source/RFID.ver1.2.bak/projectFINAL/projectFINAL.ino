/*
 * Main Function : System Intro
 * Creator: sketchpink
 * Time: 2015.10.1
 */
 
#include <MenuList.h>
#include <SPI.h>      //Already included in RFID.h
#include <Option.h>   //re-include?
 
OPTION option;        //system control
oren o;

void setup(){
  Serial.begin(9600);
  SPI.begin();
}

void loop(){
  /*  
  Serial.println("=================WELCOME=================");
  Serial.println("Menu List: up    -- DataBase Check");
  Serial.println("           left  -- Management");
  Serial.println("           right -- Consumption");
  Serial.println("           down  -- Reboot");
  Serial.println("=========================================");
  Serial.println();
  Serial.flush();
  */
  CreateUI::titleLine("WELCOME");
  CreateUI::menu("DataBase Check", "Management", "Consumption", "Reboot");
  CreateUI::endLine();
  Serial.println();
  
  
   
  o = option.getOpt();
  switch(o){
    case up: UI::Database(); break;
    case left: UI::Management(); break;
    case right: UI::Consumption(); break;
    case down: 
      CreateUI::titleLine("Restarting System...");
      Serial.println();
      break;
  }
    
  delay(500);
 
}


