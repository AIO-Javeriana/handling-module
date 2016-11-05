//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x5xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  J. Stevenson
//  Texas Instruments, Inc
//  July 2011
//  Built with Code Composer Studio v5
//***************************************************************************************

#include <msp430.h>
//#include <msp430g2452.h>


#define LED_1   (BIT1)                      // P1.0 LED output
#define LED_2   (BIT6)                      // P1.6 LED output

void changeDutyCycle(float period,float dutyCycle){
	CCR2 = period*((1-2*dutyCycle));                               // CCR2 PWM duty cycle
}

//*/
void blink(){

	for(;;) {
				volatile unsigned int i;	// volatile to prevent optimization

				P1OUT ^= BIT6;				// Toggle P1.0 using exclusive-OR

				i = 10000;					// SW Delay
				do i--;
				while(i != 0);
			}
}

void configCLK(){
	DCOCTL  =   0;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL  =  CALDCO_1MHZ;
}


int main(void) {

	float clk=5e5;
	float period=(clk/50);
	float dutyCycle=0.01;
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	configCLK();
	P1DIR |= LED_2 + BIT6 ;  	// P1.4 output
	P1OUT = 0x00;
	P1SEL |= BIT6;                            // P1.4 options
	P1SEL2 |= BIT6;
	CCR0 = period;                             // PWM Period
	CCTL2 = OUTMOD_7;                         // CCR2 reset/set


	changeDutyCycle(period,dutyCycle);
	TACTL = TASSEL_2  + MC_3;                  // SMCLK, up/down mode
	//TACCTL1 = OUTMOD_7 | CCIE;
	float i=0.04;
	while(1){
		if(i>0.1){
			i=0.04;
		}
		__delay_cycles(1000000);//1000000
		P1OUT ^= LED_2;
		dutyCycle=i;
		i=i+0.01;
		changeDutyCycle(period,dutyCycle);
	}
	_BIS_SR(CPUOFF);                          // Enter LPM0
	//*/

    // WDT is clocked by fSMCLK (1MHz)

	return 0;
}


