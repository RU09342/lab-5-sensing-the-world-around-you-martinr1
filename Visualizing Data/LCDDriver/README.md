# 3-Channel ADC Input LCD Driver

This program is loaded onto the MSP430FR6989 to display onto the LCD screen the 4-digit ADC values of 3 different ADC-input pins. These sensors are labeled as S1, S2, and S3 on the LCD screen. This program was originally designed for the 3 photo-sensors. Since only one sensor can be displayed onto the LCD screen at any given time, a refresh rate of about 0.8 seconds cycles through each of the 3 sensors indefinitely. The rightmost 4 digits represent the 4-digit ADC_light value being stored after the measured value has been converted to an ADC12 value (thousands digit, hundreds digit, tens digit, ones digit). 

### Installing on MSP430FR6989

The provided main.c code file for the MSP430FR6989 first imports driverlib.h and hal_LCD.h, complements of TI.com's resource codes for this board. The hal_LCD files and the driverlib folder must first be added to the project folder containing the main.c file being compiled in CCS. Then after wiring the 3 sensors to the 3 ADC pins (and to +3.3V Vcc and GND), the program can directly be loaded. Note: the program will take about 1-2 seconds to load characters onto the LCD screen after the program is loaded.

Pins used:

```
P8.4 = ADC pin for 3rd sensor. Channel A7. Displayed as S3 on the LCD.
P8.5 = ADC pin for 2nd sensor. Channel A6. Displayed as S2 on the LCD.
P8.6 = ADC pin for 1st sensor. Channel A5. Displayed as S1 on the LCD.
```

### Code-Breakdown

Below are concise descriptions for each of the listed "void" functions within the code:


>int main(void); = Watchdog and default GPIO high-impedance mode are turned off, and the 3 ADC pins are configured. The LCD is initialized from hal_LCD.h. The 12-bit ADC is initialized, beginning on channel A5 or the first sensor, and TimerB0 is initialized. On line 87 ("TB0CCR0 = 4000;"), this number can be increased to slow down the cycle rate of the 3 displayed ADC values, or decreased to speed up the cycle rate. Low power mode is entered, and interrupts are enabled.



>void displayStatus(int sensor); = This function goes along with the Init_LCD() funtion (to enable the showChar() function) that was run from the main(void) block. The two left-most digits "S" and "1", 2, or 3 are immediately loaded. The 4 if-statements divide the 4-digit ADC value into 4 separate single-digit values; for example the 3rd-from-the-left digit on the LCD screen has the ADC_light value divided by 1000. The 4 else-statements simply output a "0" digit if the ADC_light value does not reach up to the corresponding digit. The pos# indicators in the showChar() function begin at pos1 for the left-most digit on the LCD screen, up to the maximum pos6 as the right-most digit.



>__interrupt void Timer0_B0_ISR (void); = The ADC12 conversion is enabled and started. The very next while-loop waits until the conversion is finished. The LCD screen 6-digit display is run during the timer interrupt. Three if-statements look to see which one of the 3 sensors is currently being displayed on the LCD. In the first if-statement "if(sensor==1)" and on line 131 ("sensor+=1;"), the + symbol can be removed to force the LCD to display only one sensor and stop cycling the other two, if desired. The ADC_light variable is fed the current ADC12MEM0 value, and divided by 10, 5, or 1. These divisions can be changed for tuning to the most desired ADC range that is displayed; they do not necessarily correspond to the 3 photo-sensors, but these division do the 3 readings to be relatively close to each other under the same amount of light (along with the Interface Circuits for the photoresistor/diode/transistor). Conversion of ADC12 is turned off, the sensor variable is incremented to display the next sensor's ADC12 value, and the ADC pin's voltage measurements being converted is rerouted to a new A# channel. After each if-statement is run (before going directly to the next one and cycling the displayed sensor), the timer interrupt flag is cleared and the program is run again from main(void). 


### Software Tools Used

* [Code Composer Studio](https://dev.ti.com/) - Code compiling program (CCS). 
