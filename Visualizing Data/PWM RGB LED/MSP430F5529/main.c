#include <msp430f5529.h>

//PWM RGB LED
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    /*** GPIO Set-Up ***/
    P1DIR |= BIT2 + BIT3; // two GPIO LEDs
    P2DIR |= BIT0 + BIT4 + BIT5;
    P2SEL |= BIT0 + BIT4 + BIT5; // red RGB and green RGB and blue RGB

    /*** PWM Timer Set-Up ***/
    TA1CTL |= TASSEL_2 | MC_1 | ID_3; // SMCLK, up-mode, divide by 2^3
    TA1CCR0 |= 55; // 2^12 bits, 12 bit ADC
    TA1CCTL1 |= OUTMOD_7; //P2.0
    TA1CCR1 = 0; // red PWM
    
    TA2CTL |= TASSEL_2 | MC_1 | ID_3;
    TA2CCR0 = 55;
    TA2CCTL1 |= OUTMOD_7; //P2.4
    TA2CCTL2 |= OUTMOD_7; //P2.5
    TA2CCR1 = 0; // green PWM
    TA2CCR2 = 0; // blue PWM
    
    /*** ADC Set-Up ***/
    ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHP;                     // Use sampling timer
    ADC12IE = 0x01;                           // Enable interrupt
    P6SEL |= 0x01;                            // P6.0 ADC option select
    
    while (1)
    {
        ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
        __bis_SR_register(LPM0_bits + GIE);     // LPM0, ADC12_ISR will force exit
        __no_operation();                       // For debugger
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(ADC12IV,34))
  {
  case  6:                                  // Vector  6:  ADC12IFG0
    if (ADC12MEM0 <= 5) { // when sensor reads full darkness
            TA1CCR1 = ADC12MEM0; // set red PWM
            TA2CCR1 = (ADC12MEM0 / 3); // set green PWM
            TA2CCR2 = (ADC12MEM0 / 5); // set blue PWM
            P2OUT |= BIT3; P2OUT |= BIT2; // turn both LEDs on. 11
        }
        if ((ADC12MEM0 > 5) && (ADC12MEM0 <= 35)) { // when sensor reads partial darkness
            TA1CCR1 = ADC12MEM0;
            TA2CCR1 = (ADC12MEM0 / 3);
            TA2CCR2 = (ADC12MEM0 / 5);
            P2OUT |= BIT3; P2OUT &= ~BIT2; // 10
        }
        if((ADC12MEM0 > 35) && (ADC12MEM0 <= 50)) { // when sensor reads partial brightness
            TA1CCR1 = ADC12MEM0;
            TA2CCR1 = (ADC12MEM0 / 3);
            TA2CCR2 = (ADC12MEM0 / 5);
            P2OUT &= ~BIT3; P2OUT |= BIT2; // 01
        }
        if(ADC12MEM0 > 50) { // when sensor reads full brightness
            TA1CCR1 = ADC12MEM0;
            TA2CCR1 = (ADC12MEM0 / 3);
            TA2CCR2 = (ADC12MEM0 / 5);
            P2OUT &= ~BIT3; P2OUT &= ~BIT2; // turn both LEDs off. 00
        }
    __bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
  default: break; 
  }
}