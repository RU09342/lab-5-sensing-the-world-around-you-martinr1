#include <msp430fr6989.h>

//PWM RGB LED Dual-ADC
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

  /*** GPIO Set-Up ***/
    //P1OUT &= ~BIT0;                           // Clear LED to start
    //P1DIR |= BIT0;                            // P1.0 output
    P9SEL1 |= BIT2 + BIT3;                    // Configure P9.2 and P9.3 for ADC
    P9SEL0 |= BIT2 + BIT3;
    P3DIR |= BIT3; // PWM output
    P3SEL0 |= BIT3; // red RGB
    P2DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT6 + BIT7;
    P2OUT |= BIT0 + BIT1 + BIT2 + BIT3; // 4 GPIO LEDs
    P2SEL0 |= BIT6 + BIT7; // blue RGB and green RGB
    
    /*** PWM Timer Set-Up ***/
    TA1CTL |= TASSEL_2 | MC_1 | ID_3; // SMCLK, up-mode, divide by 2^3
    TA1CCR0 |= 55; // 2^10 bits, 12 bit ADC
    TA1CCTL1 |= OUTMOD_7; //P3.3
    TA1CCR1 = 0; // red PWM
    
    TB0CTL |= TASSEL_2 | MC_1 | ID_3;
    TB0CCR0 = 55;
    TB0CCTL5 |= OUTMOD_7; //P2.6
    TB0CCTL6 |= OUTMOD_7; //P2.7
    TB0CCR5 = 0; // green PWM
    TB0CCR6 = 0; // blue PWM

  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;

  // By default, REFMSTR=1 => REFCTL is used to configure the internal reference
  while(REFCTL0 & REFGENBUSY);              // If ref generator busy, WAIT
  REFCTL0 |= REFVSEL_0 | REFON;             // Select internal ref = 1.2V
                                            // Internal Reference ON

  // Configure ADC12
  ADC12CTL0 = ADC12SHT0_2 | ADC12ON | ADC12MSC;
  ADC12CTL1 |= ADC12SHP | ADC12CONSEQ_1;    // ADCCLK = MODOSC; sampling timer
  ADC12CTL2 |= ADC12RES_2;                  // 12-bit conversion results
  ADC12IER0 |= ADC12IE1;                    // Enable ADC conv complete interrupt
  ADC12MCTL0 |= ADC12INCH_10 | ADC12VRSEL_1; // A10 ADC input select; Vref=1.2V. Reference off for pin input. P9.2
  ADC12MCTL1 |= ADC12INCH_11 | ADC12VRSEL_1 | ADC12EOS; // A4 ADC input select; Vref=1.2V. Reference off for pin input. P9.3

  while(!(REFCTL0 & REFGENRDY));            // Wait for reference generator
                                            // to settle
  
  while(1)
  {
    __delay_cycles(50000);
    ADC12CTL0 |= ADC12ENC | ADC12SC;         // Sampling and conversion start
    __bis_SR_register(LPM0_bits + GIE);      // LPM0, ADC12_ISR will force exit
    __no_operation();                        // For debug only
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
  switch (__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
  {
    case ADC12IV_ADC12IFG0:                 // Vector 14:  ADC12MEM0 Interrupt
        //ADC_value = ADC12MEM0;
        //ADC_value2 = ADC12MEM1;
        //P1OUT ^= BIT0;
      __bic_SR_register_on_exit(LPM0_bits + GIE); // Exit active CPU after code ends
      
      /********* Photoresistor RGB LED Control *********/
      if (ADC12MEM0 <= 5) { // when sensor reads full darkness
            TA1CCR1 = ADC12MEM0; // set red PWM
            TB0CCR5 = (ADC12MEM0 / 3); // set green PWM
            P2OUT |= BIT1; P2OUT |= BIT0; // turn both LEDs on. 11
        }
        if ((ADC12MEM0 > 5) && (ADC12MEM0 <= 35)) { // when sensor reads partial darkness
            TA1CCR1 = ADC12MEM0;
            TB0CCR5 = (ADC12MEM0 / 3);
            P2OUT |= BIT1; P2OUT &= ~BIT0; // 10
        }
        if((ADC12MEM0 > 35) && (ADC12MEM0 <= 50)) { // when sensor reads partial brightness
            TA1CCR1 = ADC12MEM0;
            TB0CCR5 = (ADC12MEM0 / 3);
            P2OUT &= ~BIT1; P2OUT |= BIT0; // 01
        }
        if(ADC12MEM0 > 50) { // when sensor reads full brightness
            TA1CCR1 = ADC12MEM0;
            TB0CCR5 = (ADC12MEM0 / 3);
            P2OUT &= ~BIT1; P2OUT &= ~BIT0; // turn both LEDs off. 00
        }
    break;

    case ADC12IV_ADC12IFG1:                 // Vector 15:  ADC12MEM1 Interrupt
        //ADC_value = ADC12MEM0;
        //ADC_value2 = ADC12MEM1;
        //P1OUT ^= BIT0;
      __bic_SR_register_on_exit(LPM0_bits + GIE); // Exit active CPU after code ends
      
      /********* Photodiode RGB LED Control *********/
      if (ADC12MEM1 <= 5) { // when sensor reads full darkness
            TB0CCR6 = ADC12MEM1; // set blue PWM
            P2OUT |= BIT3; P2OUT |= BIT2; // turn both LEDs on. 11
        }
        if ((ADC12MEM1 > 5) && (ADC12MEM1 <= 35)) { // when sensor reads partial darkness
            TB0CCR6 = ADC12MEM1;
            P2OUT |= BIT3; P2OUT &= ~BIT2; // 10
        }
        if((ADC12MEM1 > 35) && (ADC12MEM1 <= 50)) { // when sensor reads partial brightness
            TB0CCR6 = ADC12MEM1;
            P2OUT &= ~BIT3; P2OUT |= BIT2; // 01
        }
        if(ADC12MEM1 > 50) { // when sensor reads full brightness
            TB0CCR6 = ADC12MEM1;
            P2OUT &= ~BIT3; P2OUT &= ~BIT2; // turn both LEDs off. 00
        }     
    break;

    default: break;
  }
}