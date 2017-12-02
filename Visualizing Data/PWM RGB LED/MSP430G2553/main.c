#include <msp430g2553.h>

//PWM RGB LED
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    /*** GPIO Set-Up ***/
    P1DIR |= BIT6; // PWM output
    P1SEL |= BIT6; // red RGB
    P2DIR |= BIT0 + BIT1 + BIT2 + BIT4; // two GPIO LEDs
    P2SEL |= BIT2 + BIT4; // blue RGB and green RGB

    /*** PWM Timer Set-Up ***/
    TA0CTL |= TASSEL_2 | MC_1 | ID_3; // SMCLK, up-mode, divide by 2^3
    TA0CCR0 |= 55; // 2^10 bits, 10 bit ADC
    TA0CCTL1 |= OUTMOD_7; //P1.2
    TA0CCR1 = 0; // red PWM
	
    BCSCTL1 = CALBC1_1MHZ;				// Set DCO to 1 MHz
	DCOCTL = CALDCO_1MHZ;
	
    TA1CTL |= TASSEL_2 | MC_1 | ID_3 | TACLR;
	TA1CCTL0 = OUTMOD_2		// compare mode
			+ CCIFG;
    TA1CCR0 = 55;
    TA1CCTL1 |= OUTMOD_7; //P2.2
    TA1CCTL2 |= OUTMOD_7; //P2.4
    TA1CCR1 = 0; // green PWM
    TA1CCR2 = 0; // blue PWM
    
    /*** ADC Set-Up ***/
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE + REFON; // ADC10ON, interrupt enabled
    ADC10CTL1 = INCH_3 + SHS_0 + ADC10DIV_3 + ADC10SSEL_3 + CONSEQ_0;	// clock source = SMCLK.
    ADC10AE0 |= BIT0;                         // PA.1 ADC option select, or P1.3

    for (;;)
    {
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
        if (ADC10MEM <= 5) { // when sensor reads full darkness
            TA0CCR1 = ADC10MEM; // set red PWM
            TA1CCR1 = (ADC10MEM / 3); // set green PWM
            TA1CCR2 = (ADC10MEM / 5); // set blue PWM
            P2OUT |= BIT1; P2OUT |= BIT0; // turn both LEDs on. 11
        }
        if ((ADC10MEM > 5) && (ADC10MEM <= 35)) { // when sensor reads partial darkness
            TA0CCR1 = ADC10MEM;
            TA1CCR1 = (ADC10MEM / 3);
            TA1CCR2 = (ADC10MEM / 5);
            P2OUT |= BIT1; P2OUT &= ~BIT0; // 10
        }
        if((ADC10MEM > 35) && (ADC10MEM <= 50)) { // when sensor reads partial brightness
            TA0CCR1 = ADC10MEM;
            TA1CCR1 = (ADC10MEM / 3);
            TA1CCR2 = (ADC10MEM / 5);
            P2OUT &= ~BIT1; P2OUT |= BIT0; // 01
        }
        if(ADC10MEM > 50) { // when sensor reads full brightness
            TA0CCR1 = ADC10MEM;	
            TA1CCR1 = (ADC10MEM / 3);
            TA1CCR2 = (ADC10MEM / 5);
            P2OUT &= ~BIT1; P2OUT &= ~BIT0; // turn both LEDs off. 00
        }
    }
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}