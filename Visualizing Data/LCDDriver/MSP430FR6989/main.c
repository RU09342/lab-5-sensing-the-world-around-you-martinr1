
/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************

/**
 * main.c
 */
#include <driverlib/MSP430FR6989/driverlib.h>
#include <msp430.h>
#include "hal_LCD.h"
int sensor =1;
int s1;
int s2;
int s3;
int ADC_light;
int ADCnew;
int ADCold;

void displayStatus(int sensor);


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;  // Disable the GPIO power-on default high-impedance mode

    /*** Configure P8.4, P8.5, P8.6 to sensor input ***/
    P8SEL1|=(BIT4+BIT5+BIT6);       // reroute 3 pins to receive 3 ADC inputs.
    P8SEL0|=(BIT4+BIT5+BIT6);

    /*** Initialize LCD from hal_LCD.h ***/
    Init_LCD();

	/*** Initialize ADC12_B ***/
    ADC12CTL0 &= ~ADC12ENC;                   // Disable ADC12
    ADC12CTL0 |= ADC12SHT0_1 + ADC12ON;       // Set sample time
    ADC12CTL1 |= ADC12SHP + ADC12CONSEQ1 + ADC12SSEL1;                     // Enable sample timer
    ADC12MCTL0 |=  ADC12INCH_5; // ADC input on channel 7 ADC12VRSEL_1 +

    /*** Initialize TimerB0 ***/
    TB0CTL= ( MC_1  + TBSSEL_1 + ID_3);             //up timer, smclk, div 8
    TB0CCTL0 = (CCIE);       // TimerB0 interrupt is enabled, interrput block will be run after _BIS_SR(CPUOFF + GIE);
	
    TB0CCR0 = 4000;        // set maximum timer value, slow down ADC input cycling and reading to LCD

    _BIS_SR(CPUOFF + GIE);        // Enter LPM0 w/ interrupt
}

void displayStatus(int sensor)
{
            /*** Set up each individual digit (6 digits) on the LCD ***/
            
            showChar('S',pos1);//left
            showChar(sensor+ '0',pos2);
            if (ADC_light>=1000)
                showChar((ADC_light/1000)%10 + '0',pos3);
            else
                showChar('0',pos3);
            if (ADC_light>=100)
                    showChar((ADC_light/100)%10 + '0',pos4);
            else
                            showChar('0',pos4);
            if (ADC_light>=10)
                    showChar((ADC_light/10)%10 + '0',pos5);
            else
                            showChar('0',pos5);
            if (ADC_light >=1)
                    showChar((ADC_light/1)%10 + '0',pos6);
            else
                            showChar('0',pos6);
}

/*ISR FOR TIMERB0 CCR0*/
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR (void)	// timer interrupt block
{
            ADC12CTL0 |= ADC12ENC + ADC12SC;        // turn on and begin ADC conversion
            while(ADC12CTL1 & ADC12BUSY);	// wait until conversion is finished
            __no_operation();			// tiny code delay for debugger
            displayStatus(sensor);		// write the characters onto the LCD that each correspond to the ADC value	

            if(sensor==1) 			// read value from first sensor
            {
                s1=ADC12MEM0/10;			//used for debugger
                ADC_light = ADC12MEM0/10;	//converts voltage to light intensity, /10 and /5 are for testing the ADC values to obtain similar values for the 3 sensors (does not necessarily create the correct range, change these values to match desired output range)
                ADC12CTL0 &= ~ADC12ENC;		//turn off and end ADC conversion
                ADC12CTL0 &=~ADC12SC;		//turn off and end ADC conversion
                sensor+=1;			//increment sensor integer number.              Note: Remove the + character before the = sign to force the LCD to display only one sensor's current ADC value.
                ADC12MCTL0 = ADC12INCH_5;	//channel A5
            }
            else if(sensor==2) 			// read value from second sensor
            {
                s2=ADC12MEM0/5;			
                ADC_light = ADC12MEM0/5;
                ADC12CTL0 &= ~ADC12ENC;		//turn off and end ADC conversion
                ADC12CTL0 &=~ADC12SC;		//turn off and end ADC conversion
                sensor+=1;			//increment sensor integer number
                ADC12MCTL0 = ADC12INCH_6;	//channel A6
            }
            else if(sensor==3) 			// read value from third sensor
            {
                s3=ADC12MEM0;			//set sensor, try photoresistor on this one.
		        ADC_light = ADC12MEM0;
                ADC12CTL0 &= ~ADC12ENC;		//turn off and end ADC conversion
                ADC12CTL0 &=~ADC12SC;		//turn off and end ADC conversion
                sensor=1;			//reset sensor integer number
                ADC12MCTL0 = ADC12INCH_7;	//channel A7
            }

            TB0CCTL0 &=~BIT0;    //clears interrupt flags
}
