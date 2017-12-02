# Photo-Sensors UART

The provided main.c code files for both the MSP430G2553 and the MSP430FR6989 are designed to measure the voltage value (between roughly +0.5V and +3V) going into the ADC pin, convert the values into a 0-100 intensity range, and transmit the values over the UART serial pins RX and TX. The program called CoolTerm was used to sync to the MSP430 boards from their serial inputs and print out their transmitted values as readable text. This coded program is designed to function with three photo-sensors: the photoresistor, photodiode, and phototransistor. The photoresistor can simply be wired as Vcc --> Photoresistor --> ADC pin input --> 1K ohm resistor --> GND to accurately print the intensity values of light emitting on the sensor. The other two sensors require circuits that convert a current source into a voltage source, and amplify (increase gain) the originally small voltage range of these two sensors for the MSP430 to receive better voltage measurements. These circuits are shown in the PCB and Schematic files (DipTrace). 

### Installing on MSP430G2553 (10-bit ADC)

Referring to the circuit schematics for the testing sensor being used, the jumper wire going to the ADC pin must be wired to P1.3 of this board. There are two small black jumpers already on board that are on the TX and RX pins; these must be rotated from vertical to horizontal in order for UART to transmit properly. Hardware is then fully set up. For confirmation, the red and green onboard LEDs should be blinking somewhat rapidly and not simultaneously. These LEDs indicate that data is being transmitted. Now, CoolTerm must be opened and configured to read the COM port that the MSP430 is currently connected to (check Device Manager for the COM#, typically COM3); baud should be 9600. If CoolTerm is set up properly, the screen should be rapidly receiving "Intensity = xxx" where xxx indicates the value within ADC10MEM.

Pins used:

```
P1.3 = ADC pin
P1.2 = TX pin
P1.1 = RX pin
P1.0 = red LED
P1.6 = green LED
```

### Installing on MSP430FR6989 (12-bit ADC)

Referring to the circuit schematics for the testing sensor being used, the jumper wire going to the ADC pin must be wired to P9.2 of this board. There are two small black jumpers already on board that are on the TX and RX pins; these must be rotated from vertical to horizontal in order for UART to transmit properly. Hardware is then fully set up. For confirmation, the red and green onboard LEDs should be blinking somewhat rapidly and not simultaneously. These LEDs indicate that data is being transmitted. Now, CoolTerm must be opened and configured to read the COM port that the MSP430 is currently connected to (check Device Manager for the COM#, typically COM3); baud should be 9600. If CoolTerm is set up properly, the screen should be rapidly receiving "Intensity = xxx" where xxx indicates the value within ADC12MEM0.

Pins used:

```
P9.2 = ADC pin
P4.2 = TX pin
P4.3 = RX pin
P1.0 = red LED
P9.7 = green LED
```

### Code-Breakdown

Below are concise descriptions for each of the listed "void" functions within the code (both MSP430 boards):

```
void init_led(); = Initializes LED pins.
```

```
void init_uart(); = Initializes UART pins and sets baud rate as 9600. Enables the TX interrupt for the MSP430G2553. For the MSP430FR6989 the TX interrupt is enabled only after the ADC is not busy converting its measured values.
```

```
void init_timer(); = Initializes the internal clock for controlling the speed of the ADC conversions and UART transmissions. the Timer0_A0 interrupt is enabled.
```

```
void init_adc(); = Initializes the ADC and sets the input channel as A3, or P1.3, for the MSP430G2553. The ADC10 is turned on and enabled, but conversions are not yet started.
```

```
void start_conversion(); = An if-statement waits for the ADC to be done converting before toggling the green LED. The value in ADC10 is converted, and the very previous ADC value is stored into an address register by the unsigned volatile int latest_adc_result. For the MSP430FR6989 there is no latest_adc_result variable, and instead the ADC12MCTL0 memory register is directly fed the ADC12 value, where a variable ADC12MEM0 can be used to represent the very previously stored ADC value.
```

```
__interrupt void Timer0_A0(void); = Creates a small if-statement timer that controls the enabling and disabling of the TX interrupt. Currently, the if-statement waits for timer_count to be greater than 1, creating a rapid transmission of ADC values over TX UART to the CoolTerm program. For a 1 second refresh rate in CoolTerm, changing the number from 1 to 16 greatly slows down the rate of values being printed. Within the if-statement, timer_count is reset, the start_conversion() block is performed, and the TX interrupt is enabled. 
```

```
__interrupt void USCI0TX_ISR(void); = The red LED is turned on, and a buffer array variable outputs the "Intensity: " characters one at a time as well as the ADC value currently stored in either latest_adc_value or ADC12MEM0. Two while-loops control the individual character-printing by waiting for the TX buffer to be ready for the next character to be transmitted (the "i" int variable is incremented per loop for moving to the next character). Once all characters have been transmitted, the red LED is turned off, and the TX interrupt flag is reset, ready and waiting for the next timer interrupt to activate it.
```

```
void main(void); = Turns off watchdog timer, calibrates the clock systems to 1 MHz, and performs the code within init_led(), init_uart(), init_timer(), and init_adc(). Interrupts are initialized, to be enabled/activated later in the code, and the CPU is put to sleep.
```


# Photo-Sensors UART 2-channel ADC

This code is almost identical to the 1-channel ADC UART from above, but instead two pins are receiving voltage values from two different sources. Currently within the code (for MSP430G2553 only), the GPIO push-button serves to switch between which ADC pin has its values transmitted when held down. The switching is controlled by two different ADC initialization blocks, a different one is loaded depending on the input from the push-button (init_adc_diode and init_adc_resistor instead of init_adc). However, there is a small issue within the code that causes solid values from either Vcc or GND to transmit over UART when the button is held, and the issue could not be resolved. Somehow, the main block has to be called again with the altered ADC parameters for switching between two ADC pins, but the debugger appears to show that the program is stuck at _bis_SR_register(GIE+LPM0_bits) while the button is held. Regardless, the code is provided for anyone who can solve the issue, which is certainly only caused by one or two lines. Perhaps the MSP430G2553 does not allow dual-channel ADC, but online resources were vague at best for dual-channeling this board.

Pins used:

```
P1.3 = ADC pin 1
P1.4 = ADC pin 2
P1.5 = GPIO push-button
P1.2 = TX pin
P1.1 = RX pin
P1.0 = red LED
P1.6 = green LED
```

### Software Tools Used

* [Code Composer Studio](https://dev.ti.com/) - Code compiling program
* [CoolTerm](http://freeware.the-meiers.org/) - Used to print characters from TX/RX UART to PC

### Miscellaneous Codes

Within the Photo-Sensors UART --> MSP430G2553 folder, a matlab script file called MATLABuartadc.m is provided. This file can be used with the adjacent main.c file for printing out the ADC values onto a plot over time. Although an example plot is not provided, the basic generated plot has a sample density equal to the number of transmitted values per second, causing a highly jagged and unsmooth plot line. Altering the speed of the clocks within the code can increase or decrease the plot's accuracy. Also, CoolTerm must not be opened when plotting in MATLAB, since the COM# serial port will be currently in use. Some other files in the (MISC) --> MSP430G2553 folder include a failed attempt at a dual-ADC UART transmission, a UART transmission for the onboard temperature sensor, a dual-LED night light program (LEDs turn on when no light is detected), and a UART standalone node that turns on/off LEDs when single-character inputs are entered in CoolTerm and sent over RX UART. 