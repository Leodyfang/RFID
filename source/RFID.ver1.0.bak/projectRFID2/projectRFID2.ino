/*
 * Storage Check function
 * Creator: sketchpink
 * Time: 2015.10.1
 */
#include <SPI.h>
#include <RFID.h>
#include <Option.h>
#include <SectorKey.h>

#include <Storage.h>


OPTION option;

Data data(1);
oren o;



void setup(){
  Serial.begin(9600);
}
void loop(){


data.serNum[0] = 'F';
data.serNum[1] = 'D';
data.serNum[2] = 'F';
data.serNum[3] = 'W';
data.serNum[4] = 'Q';

data.balance[0] = 120;
data.balance[1] = 3;
data.balance[2] = 7;
data.status = 4;
STORAGE storage(data);
o = option.getOpt();
  if (o == up) {
    storage.flushDataBase();
    
  } 
  else if (o == down) {
    for (int i = 0; i < 512; i++) { 
      Serial.print(EEPROM.read(i), DEC);
      Serial.println(); 
      delay(500);
    }
  }
  else if (o == left) {
    if (storage.create()){
      
    }
  }else if (o == right) {
    Serial.println("Total:");
    Serial.println(storage.counter(), DEC);
    Serial.println("==========");
  }
  delay(400);
}
