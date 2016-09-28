# Handling-Module

This is logic for Handling Module that is using a Microcontroller [msp430g2452](http://www.ti.com/product/MSP430G2452)(This logic developed in c
)


http://coecsl.ece.illinois.edu/ge423/datasheets/MSP430Ref_Guides/Cexamples/MSP430G2xx2%20Code%20Examples/C/



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



int main(void) {
	float period=512-1;
	float duty=0.2;
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	P1DIR |= BIT4;                            // P1.4 output
	P1SEL |= BIT4;                            // P1.4 options
	P1SEL2 |= BIT4;
	CCR0 = period;                             // PWM Period
	CCTL2 = OUTMOD_7;                         // CCR2 reset/set
	CCR2 = period*duty;                               // CCR2 PWM duty cycle
	TACTL = TASSEL_2 + MC_3;                  // SMCLK, up/down mode

	_BIS_SR(CPUOFF);                          // Enter LPM0
	 return 0;
}


