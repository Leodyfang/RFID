/*
 * CreateUI.h - createUI by function
 * Creator: sketchpink
 * Time: 2015.10.1
 */
 
#ifndef CreateUI_H
#define CreateUI_H
#endif

#define LEN 40

class CreateUI{

public:
	
	static void titleLine(String s){
		unsigned char half = (LEN - s.length()) / 2;
		
		for (int i = 1; i <= half; i++)	Serial.print('*');
		Serial.print(s);
		if(s.length() % 2){
			
			for (int i = 1; i <= half + 1; i++) Serial.print('*');
			
		} else {
			for (int i = 1; i <= half; i++) Serial.print('*');
		}
		Serial.println();
	}
	
	static void endLine(){
		
		for (int i = 1; i <= LEN; i++)	Serial.print('*');
		Serial.println();
		
	}
	
	static void menu(String up, String left, String right, String down){
		
		Serial.println((String)"1. up    -- " + up);
		Serial.println((String)"2. left  -- " + left);
		Serial.println((String)"3. right -- " + right);
		Serial.println((String)"4. down  -- " + down);
		
	}
	
};


