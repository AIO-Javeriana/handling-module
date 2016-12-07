
#ifndef TOGGLESERVICE_H
#define TOGGLESERVICE_H 
        
#include <json.hpp>
#include <iostream>
#include <HandlingModuleInfo.cpp>
#include <Services.cpp>
#include <string>

#include <wiringPi.h>
#include <wiringSerial.h>

//*/
#ifdef WIN32

#define HIGHLIGHT(__O__) std::cout<<__O__<<std::endl
#define EM(__O__) std::cout<<__O__<<std::endl
#include <stdio.h>
#include <tchar.h>

#else

#define HIGHLIGHT(__O__) std::cout<<"\e[1;31m"<<__O__<<"\e[0m"<<std::endl
#define EM(__O__) std::cout<<"\e[1;30;1m"<<__O__<<"\e[0m"<<std::endl

#endif
using json = nlohmann::json;
using namespace std;

class ToggleService: public Service{

public:
    
    MoveService():Service(){
        this->name="TOGGLE";
        this->interruptible=false;
        this->service=false;
		this->params.push_back("REPETITIONS"); 
    	Service::registerClass(name, &MoveService::create);
    
    }
    
    MoveService(string name, list<string> params, bool interruptible, bool service) :
        Service(name, params, interruptible, service) {
        Service::registerClass(name, &MoveService::create);
    }
    static Service* create() { 
        return new MoveService; 
    }
    
    bool execute(json params, string &msg,ModuleInfo* moduleInfo){
       	int repetitions=params["REPETITIONS"];
		int emotion=params["EMOTIONAL_VALUE"];
		handlingService(1) ;
		for (int i=0;i<repetitions;i++){
			delay(200+500(2-emotion+1)) ;			
			handlingService(1); 
			delay(200+500(2-emotion+1)) ;			
			handlingService(1) ;	
		}
        return true;
    }

	int handlingService(int service){
		int fd ;
		  int count ;
		  int handlingService(int service){
	      int fd ;
		  int count ;
		  unsigned int nextTime ;
		  char charReplay;
		  
		  bool sw=false;
		  if ((fd = serialOpen ("/dev/ttyS1", 9600 )) < 0)//115200
		  {
			fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
			return 1 ;
		  }

		  if (wiringPiSetup () == -1)
		  {
			fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
			return 1 ;
		  }
		cout<<"Stat"<<endl	;	
		string reply="{}";
		nextTime = millis () + 300 ;
		int countMSG = 0;
		int state=0;
		fflush (stdout) ;
		if (service==0){
			cout<<"<<<<<<<<<OPEN: " ;
			serialPuts(fd,"{\"COMMAND\":\"OPEN\"}") ;
		}else if (service==1){
			cout<<"<<<<<<<<<CLOSE: " ;	
			serialPuts(fd,"{\"COMMAND\":\"CLOSE\"}") ;
		}else if (service==2){
			cout<<"<<<<<<<<<TOGGLE: " ;	
			serialPuts(fd,"{\"COMMAND\":\"TOGGLE\"}") ;
		}
		cout<<"Stat"<<endl	;	
		reply = "";
		  for (count = 0 ; count < 256 ; )
		  {
			if (0<reply.size()&&reply[reply.size()-1]=='}'&&countMSG==0 )
			//if (millis () > nextTime)
			{
			nextTime += 300 ;
			  break; 
			}

			delay (3) ;
			//printf(" ->") ;
	
			while (serialDataAvail (fd)){
				charReplay = serialGetchar (fd); 
				if (sw || charReplay  == '{' ) {
					sw = true;		
					reply += charReplay;
					if (charReplay  == '{' ) {
						countMSG++;
					}
					if (charReplay  == '}' ) {
						countMSG--;
					}
				}
			  	printf ("%c", charReplay) ;
			  	fflush (stdout) ;
			}
			//serialFlush (fd);
			//cout<<"chao"<<endl;
		  }
		  	
		  printf ("\n") ;
		  //serialClose(fd);
			return 0;
		}


    
   
    
};
#endif
