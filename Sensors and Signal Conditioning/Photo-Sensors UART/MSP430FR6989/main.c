
#include "msp430fr6989.h"
#include <stdio.h>

//Photo-Sensors UART

unsigned short data;                // data read from ADC 

// global variables
volatile unsigned int timer_count = 0;
char buffer[32];

void init_led();
void init_uart();
void init_timer();
void init_adc();
void start_conversion();

void main(void)
{
	WDTCTL 	 = WDTPW + WDTHOLD;	// stop the WDT
	PM5CTL0 &= ~LOCKLPM5; // Disable GPIO power-on default high-impedance mode to activate configured port settings
	// Clock System Setup
    CSCTL0 = CSKEY >> 8;                    // Unlock CS registers
    CSCTL1 = DCOFSEL_0 | DCORSEL;            // Set DCO to 16MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK; // Set SMCLK = MCLK = DCO,
                                            // ACLK = VLOCLK
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL0 = 0;                             // Lock CS registers

	// initiate peripherals
	init_led();
	init_uart();
	init_timer();
	init_adc();

	// enable interrupts and put the CPU to sleep
	_bis_SR_register(GIE+LPM3_bits);
}

// HELPER FUNCTIONS
void init_led() 
{
	P1DIR = 0x01; //red LED P1.0
	P9DIR = 0x80; //green LED P9.7
}

void init_uart() 
{
	P4SEL0  = BIT3 + BIT2;	// P4.3 = RXD, P4.2 = TXD
	P4SEL1 = BIT3 + BIT2;	// P4.3 = RXD, P4.2 = TXD
	UCA0CTL1 |= UCSSEL_2;	// SMCLK
	UCA0BR0 = 104;			// see baud rate divider above
	UCA0BR1 = 0;
	UCA0MCTLW = UCBRS0;		// modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;	// ** initialize USCI state machine **
}

void init_timer() 
{
	TA0CTL |= TACLR;		// reset timer
	TA0CTL  = TASSEL_2		// SMCLK
			+ ID_0			// input divider = 1
			+ MC_2;			// continuous mode, interrupt disabled
	TA0CCTL0 = OUTMOD_2		// compare mode
			+ CCIE			// interrupt enabled
			+ CCIFG;
}

void init_adc()
{
	ADC12CTL1 = ADC12INCH_10		// photoresistor input to ADC //P9.2
			+ ADC12SHS_0			// use ADC10SC bit to trigger sampling
			+ ADC12DIV_3	// clock divider = 4
			+ ADC12SSEL_3	// clock source = SMCLK
			+ ADC12CONSEQ0;		// single channel, single conversion
	//ADC12DTC1 = 1;			// one block per transfer
    //ADC10AE0 = 0x08;      // enable A4 analog input
	//ADC12CTL0 = CEREF00		// reference voltages are Vss and Vcc. 0 allows code to work.
			+ ADC12SHT0_3	// 64 ADC10 clocks for sample and hold time (slowest)
			+ REFON			// reference generator on
			+ ADC12ON		// turn on ADC10
			+ ADC12ENC;			// enable (but not yet start) conversions
}

void start_conversion() 
{
	if ((ADC12CTL1 & ADC12BUSY) == 0) {	// if not already converting
		P9OUT ^= 0x80; //green led
		ADC12CTL0 |= ADC12SC;
		ADC12MCTL0 = ADC12VRSEL_0 | ADC12INCH_10; // Ref Vcc/Vss, channel A10, P9.2	
		//while (ADC12CTL1 & ADC12BUSY)/*EMPTY*/; //wait for conversion to finish
	}
}

// INTERRUPT HANDLERS
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void) 
{
	timer_count++;
	if (timer_count > 1) { //default is 16 for 1 second refresh raet
		timer_count = 0;
		start_conversion();
		UCA0IE |= UCTXIE0;	// Enable USCI_A0 TX interrupt after conversion
	}
}

#pragma vector = USCI_A0_VECTOR
__interrupt void USCI0TX_ISR(void) 
{
	P1OUT |= 0x01; //red led
	unsigned int i = 0;					// iterator pointers
	sprintf(buffer, "Intensity: %d \n\r", (int)(ADC12MEM0)); //output text to PC

	while (buffer[i] != '\0') {
		while (!(UCA0IFG & UCTXIFG));	// USCI_A0 TX buffer ready?
		UCA0TXBUF = buffer[i++];
	}

	P1OUT &= ~0x01; //red led
	UCA0IE &= ~UCTXIFG;
}
