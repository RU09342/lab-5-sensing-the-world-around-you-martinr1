
#include "msp430g2553.h"
#include <stdio.h>

unsigned short data;                // data read from ADC 

// global variables
volatile unsigned int latest_adc_result;
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
	BCSCTL1  = CALBC1_1MHZ;	 	// calibration for basic clock system
	DCOCTL 	 = CALDCO_1MHZ;	 	// calibration for digitally controlled oscillator

    P1REN = BIT3;
    P1DIR = BIT3;
	// initiate peripherals
	
	int mode = 0;
	if (((P1IN & BIT3) == BIT3) & (mode == 0)) {
	    init_adc_resistor();
	    init_led();
	    init_uart();
	    init_timer();
	    mode = 1;
	}
    if (((P1IN & BIT3) == BIT3) & (mode == 1)) {
	    init_adc_diode();
	    init_led();
    	init_uart();
    	init_timer();
    	mode = 0;
	}
	// enable interrupts and put the CPU to sleep
	_bis_SR_register(GIE+LPM0_bits);
}

// HELPER FUNCTIONS
void init_led() 
{
	P1DIR = 0x01 + 0x40;
}

void init_uart() 
{
	P1SEL  = BIT1 + BIT2;	// P1.1 = RXD, P1.2 = TXD
	P1SEL2 = BIT1 + BIT2;	// P1.1 = RXD, P1.2 = TXD
	UCA0CTL1 |= UCSSEL_2;	// SMCLK
	UCA0BR0 = 104;			// see baud rate divider above
	UCA0BR1 = 0;
	UCA0MCTL = UCBRS0;		// modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;	// ** initialize USCI state machine **
	IE2 |= UCA0TXIE;		// Enable USCI_A0 TX interrupt
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

void init_adc_resistor()
{
	ADC10CTL1 = INCH_3		// photoresistor input to ADC
			+ SHS_0			// use ADC10SC bit to trigger sampling
			+ ADC10DIV_3	// clock divider = 4
			+ ADC10SSEL_3	// clock source = SMCLK
			+ CONSEQ_0;		// single channel, single conversion
	ADC10DTC1 = 1;			// one block per transfer
    ADC10AE0 = 0x08;      // enable A4 analog input
	ADC10CTL0 = SREF_0		// reference voltages are Vss and Vcc. 0 allows code to work.
			+ ADC10SHT_3	// 64 ADC10 clocks for sample and hold time (slowest)
			+ REFON			// reference generator on
			+ ADC10ON		// turn on ADC10
			+ ENC;			// enable (but not yet start) conversions
}

void init_adc_diode()
{
	ADC10CTL1 = INCH_4		// photoresistor input to ADC
			+ SHS_0			// use ADC10SC bit to trigger sampling
			+ ADC10DIV_3	// clock divider = 4
			+ ADC10SSEL_3	// clock source = SMCLK
			+ CONSEQ_0;		// single channel, single conversion
	ADC10DTC1 = 1;			// one block per transfer
    ADC10AE0 = 0x10;      // enable A4 analog input
	ADC10CTL0 = SREF_0		// reference voltages are Vss and Vcc. 0 allows code to work.
			+ ADC10SHT_3	// 64 ADC10 clocks for sample and hold time (slowest)
			+ REFON			// reference generator on
			+ ADC10ON		// turn on ADC10
			+ ENC;			// enable (but not yet start) conversions
}

void start_conversion() 
{
	if ((ADC10CTL1 & ADC10BUSY) == 0) {	// if not already converting
		P1OUT ^= 0x40; //green led
		ADC10CTL0 |= ADC10SC;
		ADC10SA = (unsigned) &latest_adc_result; // store latest ADC value into address
	}
}

// INTERRUPT HANDLERS
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0(void) 
{
	timer_count++;
	if (timer_count > 1) { //default is 16 for 1 second refresh rate. set to 16 for slower data transfer
		timer_count = 0;
		start_conversion();
		IE2 |= UCA0TXIE; // activate TX interrupt
	}
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) 
{
	P1OUT |= 0x01; //red led
	unsigned int i = 0;					// iterator pointers
	sprintf(buffer, "Intensity: %d p \n\r", (int)(latest_adc_result)); //output text to PC

	while (buffer[i] != '\0') {
		while (!(IFG2 & UCA0TXIFG));	// USCI_A0 TX buffer ready?
		UCA0TXBUF = buffer[i++];
	}

	P1OUT &= ~0x01; // red led
	IE2 &= ~UCA0TXIFG; // reset interrupt flag
}
