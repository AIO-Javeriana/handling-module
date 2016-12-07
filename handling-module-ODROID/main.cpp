
#include <string>
#include <string.h>
//-----------------------
#include <stdio.h>
#include <HandlingModule.cpp> 
#include <wiringPi.h>
#include <wiringSerial.h>
using namespace std;

void start(string module_id,string host,int port);

int handlingService(int service){
		int fd ;
		  int count ;
		  unsigned int nextTime ;
		  char charReplay;
		  string reply="{}";
		  bool sw;
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

		  nextTime = millis () + 300 ;
		int countMSG = 0;
		int state=0;

				fflush (stdout) ;
				if (service==0){
					cout<<"<<<<<<<<<OPEN: " ;
					serialPuts(fd,"{\"COMMAND\":\"OPEN\"}") ;
						 	//cout<<"{\"COMMAND_ID\":3,\"COMMAND\":\"TOGGLE\",\"EVENT-NAME\":\"WORK_ASSIGNATION\",\"PARAMS\":{\"EMOTIONAL_VALUE\":1},\"GROUP_ID\":3}" <<endl;
				//serialPuts(fd,"{\"COMMAND_ID\":3,\"COMMAND\":\"TOGGLE\",\"EVENT-NAME\":\"WORK_ASSIGNATION\",\"PARAMS\":{\"EMOTIONAL_VALUE\":1},\"GROUP_ID\":3}" ) ;
					state++;		
				}else if (service==1){
		
					cout<<"<<<<<<<<<CLOSE: " ;	
					serialPuts(fd,"{\"COMMAND\":\"CLOSE\"}") ;
					state++;	
				}else if (service==2){
		
					cout<<"<<<<<<<<<TOGGLE: " ;	
					serialPuts(fd,"{\"COMMAND\":\"TOGGLE\"}") ;
					state=0;	
				}
		  for (count = 0 ; count < 256 ; )
		  {
			 
		 
			if (0<reply.size()&&reply[reply.size()-1]=='}'&&countMSG==0 )
			//if (millis () > nextTime)
			{
		
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
			return 0;
		}



int main(int argc ,const char* args[])
{   handlingService(1);
	delay(10000);
	handlingService(0);
	delay(10000);
    return handlingService(1);
    string module_id="handling_module";
    string host="ws://localhost";
    int port=9090;
    for (int i=0;i<argc;i++){
        if (strcmp (args[i],"-h")==0&& i+1<argc){
            host=args[i+1];
			host="ws://"+host;
			//host="ws://"+host;
			
        }else if (strcmp (args[i],"-n")==0&& i+1<argc){
                    module_id=(args[i+1]);
        }else if (strcmp (args[i],"-p")==0&& i+1<argc){
                    port=stoi(args[i+1]);
        }
        
    }

    cout<<module_id<<" "<<host<<" "<<port<<endl;
    start(module_id,host,port);
    return 0;
}

void start(string module_id,string host,int port){
  
  HandlingModuleInfo moduleInfo(module_id);
  HandlingModule* handlingModule = new HandlingModule(host, port,&moduleInfo);
  handlingModule->start();
  while(true);
}
