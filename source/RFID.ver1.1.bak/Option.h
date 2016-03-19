/*
 * OPTION.h - Initialize joystick as input 
 * Creator: sketchpink
 * Time: 2015.10.1
 */

#ifndef OPTION_H
#define OPTION_H


enum oren {middle, left, right, up, down};

class OPTION{
	oren opt;
	bool bounced;
	
	bool isBounce(){
		int xvalue, yvalue;
		xvalue = analogRead(0); yvalue = analogRead(1);
		
		if(opt){
			if(abs(xvalue - 500) < 250 && abs(yvalue - 500) < 250) {
				opt = middle;
				bounced = 1;
				return 1;
			}else{
				bounced = 0;
				return 0;
			}
		}
		else return 1;
	}
	bool joystickInput(){
		int xvalue, yvalue;
		
		if(isBounce()){
			xvalue = analogRead(0); yvalue = analogRead(1);
		
			if (xvalue < 250) opt = up;
			else if (xvalue > 750) opt = down;
			else if (yvalue < 250) opt = right;
			else if (yvalue > 750) opt = left;
			else opt = middle;
		}
			
		if(opt && bounced) return 1;
		else return 0;
	}
public:
	OPTION():opt(middle), bounced(1){}
	
	oren getOpt(){
		
		while(!joystickInput());
		
		return opt;
	}
	
};


#endif

