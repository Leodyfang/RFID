/*
 * MenuList.h - MenuList
 * Creator: sketchpink
 * Time: 2015.10.1
 */
 
#ifndef MenuList_H
#define MenuList_H

#include <Business.h>
#include <Option.h>
#include <CreateUI.h>

extern OPTION option;		// extern from ino project file, not included cpp files
extern oren o;

class UI{
public:
	static void Management(){
		bool loop = 1;
		
		while(loop){
			
			/*
			Serial.println("*************MANAGEMENT*******************");
			Serial.println("Function: left  -- Registration");
			Serial.println("          right -- Destory");
			Serial.println("          down  -- Return to Menu");
			Serial.println("******************************************");
			Serial.println();
			Serial.flush();
			*/
			
			CreateUI::titleLine("MANAGEMENT");
			CreateUI::menu("none", "Registration", "Destory", "Return to Menu");
			CreateUI::endLine();
			Serial.println();
			Serial.flush();
			
			o = option.getOpt();
		
			switch(o){
				case up:
					loop = 0;
					break;
					
				case left: 
					if(BUSINESS::registration()) {
						Serial.println("Registration SUCCESS!");
						BUSINESS::query();
					}
					else Serial.println("Registration FAILED, make sure it's a NEW card.");
				
					break;
					
				case right:
					if(BUSINESS::destory()) {
						Serial.println("Destory card SUCCESS! Clear record in DataBase.");
					}
					else Serial.println("Destory FAILED, no record in DataBase.");
					
					break;
					
				case down:
					loop = 0;
					break;
			}
		}
			
		delay(500);
	}
	static void Consumption(){
		bool loop = 1;
		
		while(loop){
			
			/*
			Serial.println("*************CONSUMPTION******************");
			Serial.println("Function: up    -- Query");
			Serial.println("          left  -- Deposit");
			Serial.println("          right -- Consume");
			Serial.println("          down  -- Return to Menu");
			Serial.println("******************************************");
			Serial.println();
			Serial.flush();
			*/
			
			CreateUI::titleLine("CONSUMPTION");
			CreateUI::menu("Query", "Deposit", "Consume", "Return to Menu");
			CreateUI::endLine();
			Serial.println();
			Serial.flush();
			
			o = option.getOpt();
		
			switch(o){
			
				case up: BUSINESS::query(); break;
			
				case left: BUSINESS::deposit(); break;
				
				case right: BUSINESS::consume(); break;
				
				case down: loop = 0; break;
			
			}
			
			delay(500);
		}
	}
	static void Database(){
		bool loop = 1;
			
		while(loop){
			
			/*
			Serial.println("**************DATABASE********************");
			Serial.println("Function: up    -- Flush Database.");
			Serial.println("          left  -- Print DataBase from 0 to 511 in DEC by byte.");
			Serial.println("          right -- Count Record");
			Serial.println("          down  -- Return to Menu");
			Serial.println("******************************************");
			Serial.println();
			Serial.flush();
			*/
			
			CreateUI::titleLine("DATABASE");
			CreateUI::menu("Flush Database", "Print DataBase", "Count Record", "Return to Menu");
			CreateUI::endLine();
			Serial.println();
			Serial.flush();
			
			o = option.getOpt();
		
			switch(o){
			
				case up: BUSINESS::flushDB(); break;
			
				case left: BUSINESS::printDB(); break;
				
				case right: BUSINESS::counter(); break;
			
				case down: loop = 0; break;
		
			}
		}
		
		delay(500);
	}
	
	
};


#endif

