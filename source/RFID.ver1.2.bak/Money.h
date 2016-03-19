/*
 * Money.h - balance calculate
 * Creator: sketchpink
 * Time: 2015.10.1
 */
#ifndef MONEY_H
#define MONEY_H

#define isdigit(char) ((char) >= '0' && (char) <= '9')	

class MONEY {
private:	
	 
	unsigned char cardBuffer[4];
	unsigned char serialBuffer[34];		//Arduino serial IN buffer is 64 byte, but 4 bytes double variable needs 34 char at most
	double balance;
	
	
	double atof(unsigned char s[]){
		
		double val, power;
		int i;
	
		for (i = 0; !isdigit(s[i]); i++);
	
		for (val = 0.0; isdigit(s[i]); i++)
			val = 10.0 * val + (s[i] - '0');
	
		if (s[i] == '.') i++;
	
		for (power = 1.0; isdigit(s[i]); i++){
			val = 10.0 * val + (s[i] - '0');
			power *= 10.0;
		}
	
		return val / power;	
	}
	
	inline double byte2Double(unsigned char s[]){
		
		unsigned char c[] = {s[0], s[1], s[2], s[3]};
		return *((double*)c);
	
	}
	
	inline unsigned char double2Byte(double d, int index){
	
		return ((unsigned char*)&d)[index];

	}
	  
public:
 	
	MONEY(unsigned char cb[], unsigned char sb[]){
		
		for (int i = 0; i < 4; i++)	cardBuffer[i] = cb[i];
		
		for (int i = 0; sb[i]; i++) serialBuffer[i] = sb[i];
		
		balance = byte2Double(cardBuffer);		//initialize with card money
	}
	MONEY(unsigned char cb[]){
		
		for (int i = 0; i < 4; i++)	cardBuffer[i] = cb[i];
		
		balance = byte2Double(cardBuffer);
	}
	
	double plus(){
			
		balance += atof(serialBuffer);			//plus
		
		return balance; 
	
	}
	
	double minus(){
		
		balance -= atof(serialBuffer);
			
		return balance;							//BUSINESS should control if balance was below 0.
	
	}
	
	double getBalance(){
		
		return balance;
		
	}
	
	unsigned char* getResultBuffer(){
		
		unsigned char tempbuffer[4];
		
		unsigned char* p = tempbuffer;
		
		for (int i = 0; i < 4; i++) tempbuffer[i] = double2Byte(balance, i);
		
		return p;
	}
 	
};

#endif