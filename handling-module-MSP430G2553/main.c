//******************************************************************************
//  MSP430G2xx2 Demo - Timer_A, Ultra-Low Pwr UART 9600 Echo, 32kHz ACLK
//
//  Description: Use Timer_A CCR0 hardware output modes and SCCI data latch
//  to implement UART function @ 9600 baud. Software does not directly read and
//  write to RX and TX pins, instead proper use of output modes and SCCI data
//  latch are demonstrated. Use of these hardware features eliminates ISR
//  latency effects as hardware insures that output and input bit latching and
//  timing are perfectly synchronised with Timer_A regardless of other
//  software activity. In the Mainloop the UART function readies the UART to
//  receive one character and waits in LPM3 with all activity interrupt driven.
//  After a character has been received, the UART receive function forces exit
//  from LPM3 in the Mainloop which configures the port pins (P1 & P2) based
//  on the value of the received byte (i.e., if BIT0 is set, turn on P1.0).

//  ACLK = TACLK = LFXT1 = 32768Hz, MCLK = SMCLK = default DCO
//  //* An external watch crystal is required on XIN XOUT for ACLK *//
//
//               MSP430G2xx2
//            -----------------
//        /|\|              XIN|-
//         | |                 | 32kHz
//         --|RST          XOUT|-
//           |                 |
//           |   CCI0B/TXD/P1.1|-------->
//           |                 | 9600 8N1
//           |   CCI0A/RXD/P1.2|<--------
//
//  D. Dang
//  Texas Instruments Inc.
//  December 2010
//  Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************

#include <msp430.h> 
#include <string.h>
/*
 * main.c
 */

#define LED_1   (BIT0)                      // P1.0 LED output
#define LED_2   (BIT6)                      // P1.6 LED output
#define UART_CTS_PxIN      P2IN
#define UART_CTS_PxOUT     P2OUT
#define UART_CTS_PAD   BIT0 // MSP430  CTS line  P3.2 <- BLE_RTS line
#define MIN_VALUE_SERVO 0.04
#define MAX_VALUE_SERVO 0.1

float reloj=1e6;
float periodo=20000;//clk*0.00002;
float dutyCycle=MIN_VALUE_SERVO;
unsigned char LineBuffer[100];
unsigned int bufferlength=0;
unsigned int bracketsCount=0;
void blink(){
	P1DIR |= LED_1  ;
	for(;;) {
				volatile unsigned int i;	// volatile to prevent optimization

				P1OUT ^= LED_1;				// Toggle P1.0 using exclusive-OR

				i = 10000;					// SW Delay
				do i--;
				while(i != 0);
			}
}


//------------------------------------------------------------------------------
// Function prototypes
//------------------------------------------------------------------------------
//void TimerA_UART_init(void);
void uart_tx(unsigned char byte);
void uartSend(char *string) ;
void uartnSend(char *string,int length);
void moveServo(float periodo,float dutyCycle,unsigned long wiatTime
);
//void TimerA_UART_print(char *string);
//void changeDutyCycle(float period,float dutyCycle);


int main(void) {
	  bufferlength=0;
	  bracketsCount=0;
	  WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
      //---------------------UAT-------------------
  	  // Use Calibration values  for 1MHz    Clock   DCO
  	  DCOCTL  =   0;
  	  BCSCTL1 = CALBC1_1MHZ;
  	  DCOCTL  =  CALDCO_1MHZ;
  	  P1SEL  =   BIT1    |   BIT2   ;
  	  P1SEL2 =   BIT1    |   BIT2;
  	  // Place   UCA0    in  Reset   to  be  configured*/
  	  UCA0CTL1   =   UCSWRST;
   	  // Configure
  	  UCA0CTL1   |=  UCSSEL_2;   //  SMCLK
  	  // 1MZ Clk
  	  UCA0BR0    =  104;// 16*104;    //  1MHz    9600UBR00=0x82; UBR10=0x06; UMCTL0=0x5B; // uart0 16000000Hz 9599bps
  	  UCA0BR1    =   0;  //  1MHz    9600UBR01=0x82; UBR11=0x06; UMCTL1=0x5B; // uart1 16000000Hz 9599bps
  	  UCA0MCTL   =   UCBRS0; //  Modulation  UCBRSx  =   1*/
  	  /*
  	  // 16Mz CLK
  	  UCA0BR0 = 130;        // 16MHz 9600
  	  UCA0BR1 = 6;          // 16MHz 9600
  	  UCA0MCTL = UCBRS1 + UCBRS0; // Modulation UCBRSx = 3*/
  	  // Take    UCA0    out of  reset   /
  	  UCA0CTL1   &=  ~UCSWRST;
  	  //   UCA0CTL1 = UCSWRST;                         // Hold USCI in reset to allow configuration
  	  UCA0CTL0 = 0;                               // No parity, LSB first, 8 bits, one stop bit, UART (async)
  	  // Enable  USCI_A0 RX  interrupt
  	  IE2    |=  UCA0RXIE;

  	  BCSCTL1 |= XT2OFF | DIVA_0;
  	  BCSCTL3 = XT2S_0 | LFXT1S_2 | XCAP_1;
  	  P1DIR |= BIT6;					// P1.6 set as output (Green LED)
  	  P1SEL |= BIT6;					// P1.6 selected Timer0_A Out1 output
  	  P2DIR |= BIT2;					// P1.6 set as output (Green LED)
  	  P2SEL |= BIT2;					// P1.6 selected Timer0_A Out1 output
  	  TA0CCTL1 = CM_0 | CCIS_0 | OUTMOD_3;
  	  TA1CCTL1 = CM_0 | CCIS_0 | OUTMOD_3;
  	  TA1CCTL2 = CM_0 | CCIS_0 | OUTMOD_3;

  	  TA0CCR0 = periodo; // Cargamos el periodo PWM
  	  TA1CCR0 = periodo; // Cargamos el periodo PWM
  	  TA0CCR1 = periodo*(1-dutyCycle);
  	  TA1CCR1 = periodo*(1-dutyCycle);

  	  TA0CTL = TASSEL_2 | ID_0 | MC_1;
  	  TA1CTL = TASSEL_2 | ID_0 | MC_1;

  	  __bis_SR_register(LPM0_bits + GIE);        // LPM0 (low power mode) interrupts enabled


  	}

void changeDutyCycle(float period,float dutyCycle){
	//TACCR2 = period*((1-2*dutyCycle));                               // CCR2 PWM duty cycle

	/*
	 periodoPWM_Moto=(reloj/((periodoPWM_Moto)*3));
					TA0CCR0 =periodoPWM_Moto ; // PWM Period 50us (substract 1 because it's 0-based)
					TA0CCR1 =periodoPWM_Moto*(DC_PWM_Motor1); // CCR1 PWM duty cycle
	 * */
}




  //  Echo    back    RXed    character,  confirm TX  buffer  is  ready   first
 #pragma vector=USCIAB0RX_VECTOR
 __interrupt void    USCI0RX_ISR(void)
 {
	while  (!(IFG2&UCA0TXIFG));    //  USCI_A0 TX  buffer  ready?
	UCA0TXBUF  =   UCA0RXBUF&63;  //  TX  -&amp;gt;   RXed    character
	char dato=(UCA0RXBUF),reply='1';//unsigned
	if(dato=='{' ){
		LineBuffer[bufferlength]=dato;
		bufferlength++;
  	    bracketsCount++;
	}else if(dato == '}' && bufferlength > 0 && bracketsCount==1
			){
		LineBuffer[bufferlength]=dato;
		bufferlength++;
		if(dutyCycle>MAX_VALUE_SERVO){
			dutyCycle=MIN_VALUE_SERVO;
		}
		moveServo(periodo,dutyCycle,100000);
		dutyCycle=dutyCycle+0.01;
		uartSend(LineBuffer);
		bufferlength=0;
  	    bracketsCount--;
	}else if(bufferlength > 0){
		LineBuffer[bufferlength]=dato;
		bufferlength++;
		if (dato == '}'){
			bracketsCount--;
		}
	}

 }

 //------------------------------------------------------------------------------
 // Outputs one byte using the UART
 //------------------------------------------------------------------------------
 void uart_tx(unsigned char byte)
 {
	 unsigned int txData;                        // UART internal variable for TX

     txData = byte;                          // Load global variable
     //txData |= 0x100;                        // Add mark stop bit to TXData
     //txData <<= 1;                           // Add space start bit
     while  (!(IFG2&UCA0TXIFG));    //  USCI_A0 TX  buffer  ready?
	 UCA0TXBUF  =  byte ;  //  TX  -&amp;gt;   RXed    character

    //while(UCA0STAT & UCBUSY); //UCBUSY indicates if USCI transmitting or receiving
 }


void uartSend(char *string) {

	 while (*string) {
		   // Wait for TX buffer to be ready for new data
		   while(!(IFG2 & UCA0TXIFG));//check if not set
		   while(UART_CTS_PxIN & UART_CTS_PAD); //what for the clear to send signal
		                          //wait until Tx buffer is empty
	       // Push data to TX buffer
		   UCA0TXBUF = *string++;
	 }
   // Wait until the last byte is completely sent
  while(UCA0STAT & UCBUSY); //UCBUSY indicates if USCI transmitting or receiving
 }


void uartnSend(char *string,int length) {
	int i=0;
	for(i;i<length;i++){
		   // Wait for TX buffer to be ready for new data
		   while(!(IFG2 & UCA0TXIFG));//check if not set
		  // while(UART_CTS_PxIN & UART_CTS_PAD); //what for the clear to send signal   //wait until Tx buffer is empty
	       // Push data to TX buffer
		   UCA0TXBUF = *string++;
	 }
   // Wait until the last byte is completely sent
    while(UCA0STAT & UCBUSY); //UCBUSY indicates if USCI transmitting or receiving
 }

void moveServo(float periodo,float dutyCycle, unsigned long wiatTime){
	//P1OUT ^= LED_2;
	TA0CCR1 = periodo*(1-dutyCycle);
	TA1CCR1 = periodo*(1-dutyCycle);
	dutyCycle=dutyCycle+0.01;
	__delay_cycles(100000);//1000000
	//__delay_cycles(wiatTime);//1000000
}


