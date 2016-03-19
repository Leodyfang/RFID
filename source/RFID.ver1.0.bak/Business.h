/*
 * Business.h - registration, destory, consume, deposit, query, flush, printDB, count
 * Creator: sketchpink
 * Time: 2015.10.1
 */
#ifndef BUSINESS_H
#define BUSINESS_H


#include <RFID.h>
#include <SectorKey.h>
#include <Storage.h>
#include <Money.h>

class BUSINESS{
	
public:
	static bool registration(){
		bool status;
		unsigned char index;
		unsigned char indexArray[16] = {0, 'X', 'X', 'G', 'C', 'X'};
		unsigned char balanceArray[16] = {0};
			
		RFID rfid(10, 5);
		
		rfid.init();
		rfid.isCard();
		status = rfid.readCardSerial();
		rfid.selectTag(rfid.serNum);
	 	
		if (status) {
			Data data;
			for(int i = 0; i < 5; i++){
				data.serNum[i] = rfid.serNum[i]; 
			}
			data.status = 1;
			
			STORAGE storage(data);
			index = storage.create();
		
			if(index) {
				data.index = index;
			
				indexArray[0] = data.index;
				if (rfid.auth(PICC_AUTHENT1A, SECTORKEY, sectorKeyA[1], rfid.serNum) == MI_OK) {  
  		  		
					rfid.write(INDEXBLOCK, indexArray);		//16 byte all write in 
					rfid.write(BALANCEBLOCK, balanceArray);
    	
				}
			
				rfid.halt();
				return 1;
			} else {
				rfid.halt();
				return 0;
			}
		} else {
			rfid.halt();
			return 0;
		}


	}
	static bool destory(){
		bool status;
		unsigned char indexArray[16] = {0, 'X', 'X', 'G', 'C', 'X'};
			
		RFID rfid(10, 5);
		
		rfid.init();
		rfid.isCard();
		status = rfid.readCardSerial();
		rfid.selectTag(rfid.serNum);
		if (rfid.auth(PICC_AUTHENT1A, SECTORKEY, sectorKeyA[1], rfid.serNum) == MI_OK) { 
  		    	
			rfid.read(INDEXBLOCK, indexArray);
    			
		}
		
		if (status) {
						  		
			Data data(indexArray[0]);	//construct with index
			STORAGE storage(data);
  		
			if (!storage.read()) {
				rfid.halt();
				return 0;
			} else {
				storage.writeIndex(0);
				
				indexArray[0] = 64;		//clear the card index, 64 > 51, as a clear mark, ASCII '@' 
				if (rfid.auth(PICC_AUTHENT1A, SECTORKEY, sectorKeyA[1], rfid.serNum) == MI_OK) {  
  		  		
					rfid.write(INDEXBLOCK, indexArray);
				    	
				}
				
				rfid.halt();
				return 1;
			}
		} else {
			rfid.halt();
			return 0;
		}
		
	}
	static void query(){
		bool status;
		unsigned char indexArray[16] = {0, 'X', 'X', 'G', 'C', 'X'};
		unsigned char balanceArray[16] = {0};
		
		RFID rfid(10, 5);
		
		Serial.println("*********************QUERY**********************");
		
		rfid.init();
		rfid.isCard();
		rfid.readCardSerial();
		rfid.selectTag(rfid.serNum);
		if (rfid.auth(PICC_AUTHENT1A, SECTORKEY, sectorKeyA[1], rfid.serNum) == MI_OK) { 
  		    	
			rfid.read(INDEXBLOCK, indexArray);
			rfid.read(BALANCEBLOCK, balanceArray);
			
		}
		rfid.halt();
		
		Data data(indexArray[0]);
		STORAGE storage(data);
		
		if (!storage.read()) {
			Serial.println("************No record in DataBase.**************");
			Serial.println("************************************************");
			return;
		}
		
		MONEY money(balanceArray);
				
		Serial.print("1. Card serNum is: ");
		for(int i = 0; i < 5; i++) Serial.print(data.serNum[i], HEX);	//include validate byte
		Serial.println("");
		Serial.print("2. Card index is: ");
		Serial.println(data.index);
		Serial.print("3. Reserve info is: ");
		Serial.print((char*)data.reserve);
		Serial.println("");
		Serial.print("4. Card status is:  ");
		Serial.println(data.status);
		Serial.print("5. Card balance is:  ");
		Serial.println(money.getBalance());
		Serial.println("************************************************");
		Serial.flush();
			
	}
	static void deposit(){
		bool status;								//not for return value
		unsigned char indexArray[16] = {0, 'X', 'X', 'G', 'C', 'X'};
		unsigned char balanceArray[16] = {0};
		unsigned char serialArray[34] = {0};		//Arduino serial IN buffer is 64 byte, but 4 bytes double variable needs 34 char at most
		double balance;
		unsigned char* result;						//fuck~
		
		Serial.println("****************DEPOSIT*************************");
		Serial.println("HINT: Please attach the card to the Reader.");
		
		RFID rfid(10, 5);
		
		rfid.init();								//for MFRC522, not for card procedure
		rfid.isCard();
		rfid.readCardSerial();
		rfid.selectTag(rfid.serNum);
		if (rfid.auth(PICC_AUTHENT1A, SECTORKEY, sectorKeyA[1], rfid.serNum) == MI_OK) { 
  		    	
			rfid.read(INDEXBLOCK, indexArray);
			rfid.read(BALANCEBLOCK, balanceArray);
			
		}
		rfid.halt();
		
		Data data(indexArray[0]);
		STORAGE storage(data);
		
		if (!storage.read()) {
			Serial.println("***********No record in DataBase.***************");
			Serial.println("************************************************");
			return;
		}
		
		MONEY moneyQuery(balanceArray);			//moneyQuery for query balance
		
		
		Serial.print("Card recognized.Card balance is:  ");
		Serial.println(moneyQuery.getBalance());
		Serial.println("");
		

		Serial.println("HINT:Input the deposit number through the "); // function atof() is a private member in MONEY, cant print input data
		Serial.println("     Serial Port and press SEND to confirm.");
		Serial.println("     ATTENTION:Only 0~9 and . is allowed."); // function atof() is a private member in MONEY, cant print input data
		Serial.flush();
		
		while (!Serial.available());
		
		unsigned char* p = serialArray;
		
		while (Serial.available() > 0){
			
			*p++ = (unsigned char)Serial.read();
		
			delay(2);
		}
		
		
		MONEY moneyCal(balanceArray, serialArray);
		balance = moneyCal.plus();						//return in float, for read, possible
		result = moneyCal.getResultBuffer();			//return in Array, for transform
		
		for (int i = 0; i < 4; i++, result++) balanceArray[i] = *result;	//fuck!
		
		rfid.init();
		rfid.isCard();								//AGAIN!
		status = rfid.readCardSerial();
		rfid.selectTag(rfid.serNum);
		if (rfid.auth(PICC_AUTHENT1A, SECTORKEY, sectorKeyA[1], rfid.serNum) == MI_OK) { 
  			
			rfid.write(BALANCEBLOCK, balanceArray);
			
		}
		
		if(status){
			Serial.print("Deposit Success! Current balance is:  ");
			Serial.println(balance);
			Serial.println("************************************************");
			
			rfid.halt();
			
		} else {
			Serial.println("WARNING:Communication Failed!");
			Serial.println("************************************************");
			
			rfid.halt();
			
		}

	}
	static void consume(){
		bool status;								//not for return value
		unsigned char indexArray[16] = {0, 'X', 'X', 'G', 'C', 'X'};
		unsigned char balanceArray[16] = {0};
		unsigned char serialArray[64] = {0};		//Arduino's IN serial buffer is 64 Byte
		double balance;
		unsigned char* result;						//fuck~
		
		Serial.println("*******************CONSUME***********************");
		Serial.println("HINT: Please attach the card to the Reader.");
		
		RFID rfid(10, 5);
		
		rfid.init();								//for MFRC522, not for card procedure
		rfid.isCard();
		rfid.readCardSerial();
		rfid.selectTag(rfid.serNum);
		if (rfid.auth(PICC_AUTHENT1A, SECTORKEY, sectorKeyA[1], rfid.serNum) == MI_OK) { 
  		    	
			rfid.read(INDEXBLOCK, indexArray);
			rfid.read(BALANCEBLOCK, balanceArray);
			
		}
		rfid.halt();
		
		Data data(indexArray[0]);
		STORAGE storage(data);
		
		if (!storage.read()) {
			Serial.println("***********No record in DataBase.***************");
			Serial.println("************************************************");
			return;
		}
		
		MONEY moneyQuery(balanceArray);			//moneyQuery for query balance
		
		
		Serial.print("Card recognized.Card balance is:  ");
		Serial.println(moneyQuery.getBalance());
		Serial.println("");
		

		Serial.println("HINT:Input the CONSUME number through the "); // function atof() is a private member in MONEY, cant print input data
		Serial.println("     Serial Port and press SEND to confirm.");
		Serial.println("     ATTENTION:Only 0~9 and . is allowed."); // function atof() is a private member in MONEY, cant print input data
		Serial.flush();
		
		while (!Serial.available());
		
		unsigned char* p = serialArray;
		
		while (Serial.available() > 0){
			
			*p++ = (unsigned char)Serial.read();
		
			delay(2);
		}
		
		
		MONEY moneyCal(balanceArray, serialArray);
		balance = moneyCal.minus();
		if(balance < 0) {								//abs() is better , but...fuck!
			Serial.println("**************No enough money!******************");
			Serial.println("************************************************");
			return;
		}						
		result = moneyCal.getResultBuffer();			//return in Array, for transform
		
		for (int i = 0; i < 4; i++, result++) balanceArray[i] = *result;	//fuck!
		
		rfid.init();
		rfid.isCard();								//AGAIN!
		status = rfid.readCardSerial();
		rfid.selectTag(rfid.serNum);
		if (rfid.auth(PICC_AUTHENT1A, SECTORKEY, sectorKeyA[1], rfid.serNum) == MI_OK) { 
  			
			rfid.write(BALANCEBLOCK, balanceArray);
			
		}
		
		if(status){
			Serial.print("Consume Success! Current balance is:  ");
			Serial.println(balance);
			Serial.println("************************************************");
			
			rfid.halt();
			
		} else {
			Serial.println("WARNING:Communication Failed!");
			Serial.println("************************************************");
			
			rfid.halt();
			
		}

	}
	
	
	//===================================================================================================
	
	static void flushDB(){
		STORAGE::flushDataBase();
		Serial.println("****************Data Flushed.*******************");
	}
	
	static void printDB(){
		Serial.println("****************DATABASE CONTENT****************");
		STORAGE::printDataBase();
	}
	
	static void counter(){
		
		Serial.print("The record number in DataBase is: ");
		Serial.println(STORAGE::countDataBase());
		
	}
};

#endif

