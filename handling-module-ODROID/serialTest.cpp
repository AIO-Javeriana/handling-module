/*
 * serialTest.c:
 *	Very simple program to test the serial port. Expects
 *	the port to be looped back to itself
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

using namespace std;

int main ()
{
  wiringPiSetup () ;
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

  for (count = 0 ; count < 256 ; )
  {
	 
 
	if (0<reply.size()&&reply[reply.size()-1]=='}' )
	//if (millis () > nextTime)
    {
		
	  cout<<reply<<" + "<<reply.size() ;
      reply = "";
      printf ("\nOut: %3d: ", count) ;
      fflush (stdout) ;	
		/*
      serialPutchar   (fd,'{' ) ;
      serialPutchar   (fd,'a' ) ;
      serialPutchar   (fd,'a' ) ;
      serialPutchar   (fd,'}' ) ;
		//*/
	  serialPuts(fd,"{hola que te cuentas }" ) ;
      nextTime += 300 ;
      ++count ;
	  sw = false;
    }

    delay (3) ;
	//printf(" ->") ;
	
    while (serialDataAvail (fd)){
		charReplay = serialGetchar (fd); 
		if (sw || charReplay  == '{') {
			sw = true;		
			reply += charReplay;
		}
      	//printf ("%c", charReplay) ;
      	fflush (stdout) ;
    }
	//serialFlush (fd);
	//cout<<"chao"<<endl;
  }
  	
  printf ("\n") ;
  return 0 ;
}
