/**
 * This code uses analog-to-digital converter (ADC) and an external light sensor
 * to determine the ambient brightness, and then light LEDs in response to the
 * changes in brightness. 
 * 
 * The sensed brightness will be reported using 2 LEDs, as shown below:
 *  - Extreme brightness:   both LEDs off
 *  - Moderate brightness:  green LED (LED2) on
 *  - Moderate darkness:    red LED (LED1) on
 *  - Extreme darkness:     both LEDs on
 *
 * The following external hardware is required:
 *   - photo-resistor between P1.1 and P1.2
 *
 *
 * I am using the internal pull-up resistor on the ADC input pin to form a
 * voltage divider with the photo-resistor, which is grounded through a digital
 * pin. This allows me to have minimal external components (only the photo-
 * resistor). Note that it is unconventional to use the internal pull-up in
 * this way, and typically the pull-up/downs should be disabled on any analog
 * input pins.

#include "msp430.h"                     /* include MSP430 definitions */


/* **** definitions **** */
#define SAMPLE_PERIOD   0x7FFF          /* clocks between ADC sampling */

/* LED indicator thresholds
 * these may need to be adjusted depending on specific sensor or 
 * lighting conditions */
#define EXT_BRIT    255                 /* threshold for extremely bright */
#define MOD_BRIT    511                 /* threshold for moderately bright */
#define MOD_DARK    767                 /* threshold for moderately dark */
/* anything greater is considered extremely dark */


/* pinout */
#define LED1            BIT0            /* LED1 is on P1.0 */
#define LED2            BIT6            /* LED2 is on P1.6 */
#define SENSOR          BIT1            /* sensor input on P1.1 */
#define SENSOR_GND      BIT2            /* connect sensor to GND on P1.2 */

/* shortcuts */
#define RED_LED         LED1
#define GRN_LED         LED2


/** mainloop */
void main(void) {
    
    unsigned short data;                /* data read from ADC */
    
    
    /* *************************************************************************
     * initialize system
     **************************************************************************/

    /* disable global interrupts - don't need interrupts on this project */
    __disable_interrupt();

    /* disable WDT so that the processor does not reset */
    WDTCTL = WDTPW | WDTHOLD;

    /* *************************************************************************
     * configure pins
     **************************************************************************/
        
    /* configure all pins as digital inputs with pull-downs (to save power) */
    
    /* PORT 1 */
    P1DIR = 0;                          /* set as inputs */
    P1SEL = 0;                          /* set as digital I/Os */
    P1OUT = 0;                          /* set resistors as pull-downs */
    P1REN = 0xFF;                       /* enable pull-down resistors */
    /* PORT 2 */
    P2DIR = 0;                          /* set as inputs */
    P2SEL = 0;                          /* set as digital I/Os */
    P2OUT = 0;                          /* set resistors as pull-downs */
    P2REN = 0xFF;                       /* enable pull-down resistors */

    
    /* now configure the pins that we will actually use */
    
    /* configure both LEDs as digital outputs */
    P1REN &= ~(LED1 | LED2);            /* disable pull-up/downs */
    P1DIR |= (LED1 | LED2);             /* configure as oututs */
    
    /* configure the sensor ground pin */
    P1REN &= ~(SENSOR_GND);             /* disable pull-up/down */
    P1OUT &= ~(SENSOR_GND);             /* SENSOR_GND should be at GND */
    P1DIR |= SENSOR_GND;                /* SENSOR_GND must be an output */
    
    /* configure the sensor pin as an ADC pin with a pull-up resistor
     *
     * Note: this is quite strange to do in normal situations, I am just trying
     * to use the internal pull-up resistor rather than putting my own on the
     * board */
    P1REN |= SENSOR;                    /* enable pull-up on SENSOR */
    P1OUT |= SENSOR;                    /* set resistor as pull-up */
    
    /* *************************************************************************
     * configure ADC
     **************************************************************************/
    
    /* configure ADC using the following parameters:
     *  - Vcc and GND references
     *  - 16 clock sample and hold
     *  - use A1 input
     *  - clock source: MCLK/8
     *  - single conversion mode under software control
     *  - 10-bit unsigned value
     */
    ADC10CTL0 = SREF_0 | ADC10SHT_2 | ADC10ON;
    ADC10CTL1 = INCH_1 | ADC10DIV_7 | ADC10SSEL_2;
    ADC10AE0 = SENSOR;

    
    /* *************************************************************************
     * run mainloop
     **************************************************************************/
    
    while (1) {                         /* mainloop never exits */
      
        /* start ADC conversion */
        ADC10CTL0 |= (ENC | ADC10SC);
        
        /* wait for ADC conversion to complete */
        while (ADC10CTL1 & ADC10BUSY);
        
        /* grab data from ADC */
        data = ADC10MEM;                /* this is a 10-bit unsigned value */
        
        /* change LEDs to indicate sensor value */
        if (data <= EXT_BRIT) {         /* extremely bright */
            /* turn off both LEDs */
            P1OUT &= ~(RED_LED | GRN_LED);
        } else if (data <= MOD_BRIT) {  /* moderately bright */
            /* turn on only the green LED */
            P1OUT &= ~(RED_LED);
            P1OUT |= GRN_LED;
        } else if (data <= MOD_DARK) {  /* moderately dark */
            /* turn on only the red LED */
            P1OUT &= ~(GRN_LED);
            P1OUT |= RED_LED;
        } else {                        /* extremely dark */
            /* turn on both LEDs */
            P1OUT |= (RED_LED | GRN_LED);
        }
            
        /* wait before sampling ADC again */
        __delay_cycles(SAMPLE_PERIOD);
    }
}
