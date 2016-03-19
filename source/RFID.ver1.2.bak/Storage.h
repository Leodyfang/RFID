/*
 * Storage.h - Data structure management, read/write to EEPROM
 * Creator: sketchpink
 * Time: 2015.10.1
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <EEPROM.h>		//0 ~ 511

#endif

typedef struct Data { 
	unsigned char index;	//0: Null; other: 1 ~ 51
	unsigned char serNum[5];
 	unsigned char reserve[3];
	unsigned char status;
	
	Data(){
		index = 255;
		for(int i = 0; i < 5; i++)	serNum[i] = 0;
		for(int i = 0; i < 3; i++)	reserve[i] = 0;
		status = 0;
	}
	Data(unsigned char num):index(num){
		for(int i = 0; i < 5; i++)	serNum[i] = 0;
		for(int i = 0; i < 3; i++)	reserve[i] = 0;
		status = 0;
	}
	void setIndex(unsigned char num){
		index = num;
	}
 	
}Data; //Total 10Byte 

class STORAGE {
	unsigned char ptr;		//0 ~ 50
	Data& data;
	
	void ptr2Zero() {
		ptr = 0;
	}
public:
	STORAGE(Data& d):data(d), ptr(0){}	
		
	bool read(){
		ptr2Zero();
		for(; ptr <= 50 && EEPROM.read(ptr * 10) != data.index; ptr++);
		if(ptr > 50 || !EEPROM.read(ptr * 10)) return 0;
		else {
			for(int i = 0; i < 5; i++) 
				data.serNum[i] = EEPROM.read(ptr * 10 + 1 + i); 
			for(int i = 0; i < 3; i++)
				data.reserve[i] = EEPROM.read(ptr * 10 + 6 + i);
			data.status = EEPROM.read(ptr * 10 + 9);
			return 1;
		}
	}
	bool write(){
		ptr2Zero();
		for(; ptr <= 50 && EEPROM.read(ptr * 10) != data.index; ptr++);
		if(ptr > 50 || !EEPROM.read(ptr * 10)) return 0;
		else {
			for(int i = 0; i < 5; i++) 
				EEPROM.write(ptr * 10 + 1 + i, data.serNum[i]); 
			for(int i = 0; i < 3; i++)
				EEPROM.write(ptr * 10 + 6 + i, data.reserve[i]);
			EEPROM.write(ptr * 10 + 9, data.status);
			
			return 1;
		}
	}
	bool writeIndex(unsigned char n){
		ptr2Zero();
		for(; ptr <= 50 && EEPROM.read(ptr * 10) != data.index; ptr++);
		if(ptr > 50 || !EEPROM.read(ptr * 10)) return 0;
		else {
			EEPROM.write(ptr * 10, n);
			return 1;
		}
	}
	unsigned char create(){
		
		unsigned char validate[5] = {0};	//each read from DB
		unsigned char tempPtr = 0;			//traverse in this function
		unsigned char status;
		
		for (ptr2Zero(); ptr <= 50 && EEPROM.read(ptr * 10); ptr++);	//locate
		if (ptr > 50) status = 0; // No enough free block
		else {
			
			for (; tempPtr <= 50; tempPtr++){
				
				if (!EEPROM.read(tempPtr * 10)) continue;	//data invalid
				else {										//data exist
					for(int i = 0; i < 5; i++)	validate[i] = EEPROM.read(tempPtr * 10 + 1 + i); 
					
					unsigned char* p = data.serNum;
					unsigned char* q = validate;
					unsigned char counter = 0;
					
					for (; counter <= 5 && *p++ == *q++; counter++);	//stopped when not equal 
					
					if (counter == 5) break;				//match!
					
				}
			}
			if (tempPtr == 51) {					// All checked
				EEPROM.write(ptr * 10, ptr + 1);	// index == ptr + 1
				for(int i = 0; i < 5; i++) 
					EEPROM.write(ptr * 10 + 1 + i, data.serNum[i]);
				for(int i = 0; i < 3; i++)
					EEPROM.write(ptr * 10 + 6 + i, data.reserve[i]);
				EEPROM.write(ptr * 10 + 9, data.status);
				
				status = ptr + 1;
			}
			else status = 0;
						
		}
		return status;
	}
	bool erase(){
		ptr = data.index - 1;
		if (ptr <= 50) {
			EEPROM.write(ptr, 0);
			return 1;
		}
		return 0;		
	}
	
	static unsigned char countDataBase(){
		unsigned char counter = 0;
		
		for (int i = 0; i <= 500; i += 10){
			if(EEPROM.read(i)) counter++;
		}
		
		return counter;
		
	}
	static void printDataBase(){
		for (int i = 0; i < 512; i++) { 
			Serial.print(EEPROM.read(i), DEC);
			Serial.println(); 
			delay(500);
		}
	}
	static void flushDataBase(){
		int address = 0;
		for (; address < 512; address++) EEPROM.write(address, 0);
	}
	
};


