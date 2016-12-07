#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>




using namespace std;

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

bool execute(int repetitions, int emotion){
      handlingService(1) ;
		for (int i=0;i<repetitions;i++){
			delay(200+500*(2-emotion+1)) ;			
			handlingService(0); 
			delay(200+500*(2-emotion+1)) ;			
			handlingService(1) ;	
		}
        return true;
    }
int main(int argc ,const char* args[])
{
	
execute(5,-1);
	return 0;
}
