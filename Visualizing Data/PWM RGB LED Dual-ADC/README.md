# PWM RGB LED Dual-ADC

The provided main.c code file for the MSP430FR6989 is designed to measure the voltage values (between roughly +0.5V and +3V) going into two separate ADC pins, and convert the two measured ADC values into three different PWM outputs for illuminating an RGB LED. The ADC pins are wired to any 2 of the 3 photo-sensors; no additional conversion/amplification is needed since the circuits for each photo-sensor amplify the voltage ranges to be in the same range as the photoresistor. When the first sensor (labeled photoresistor in the code) detects mostly darkness the RGB LED illuminates a dark brown, and when the sensor detects mostly brightness the RGB LED illuminates a bright orange (while the second sensor detects zero light). When the second sensor (labeled photodiode in the code) detects mostly darkness the RGB LED illuminates a dark blue, and when the sensor detects mostly brightness the RGB LED illuminates a bright and solid blue (while the first sensor detects zero light). There are also four GPIO LED output pins placed in the code as well; these simulate night-light lights. The two sets of two white GPIO LEDs (two for each sensor) indicate in binary which level of brightness is currently being detected (4 levels: bright 00, somewhat bright 01, somewhat dark 10, dark 11; in binary). 

### Installing on MSP430FR6989

Only jumper wires are required to wire the photoresistor and photodiode (or other sensor) circuits to the two ADC pins and to wire the RGB LED to the three PWM output pins. The longest RGB LED wire is wired to GND. Jumper wires are also needed to wire two white LEDs to the GPIO output pins.

Pins used:

```
P9.2 = ADC pin for 1st sensor
P9.3 = ADC pin for 2nd sensor
P3.3 = PWM output to red LED on RGB LED
P2.6 = PWM output to green LED on RGB LED
P2.7 = PWM output to blue LED on RGB LED
P2.0 = GPIO white LED for 1st sensor
P2.1 = GPIO white LED for 1st sensor
P2.2 = GPIO white LED for 2nd sensor
P2.3 = GPIO white LED for 2nd sensor
```

### Code-Breakdown

Below are concise descriptions for each of the listed "void" functions within the code:

```
int main(void); = The GPIO LED pins (including the 3 PWM output pins) are initialized, the 3 RGB PWM timers are initialized, and the two ADC input pins are initialized. A while-loop is harshly slowed down by 50000 iterations at an internal-clock cycling rate. The ADC sampling and conversion of available voltage occurs, and the ADC interrupt is force exited. As long as the code remains in the main(void) block, the ADC will continue generating new digital values for the voltage being measured.
```

```
__interrupt void ADC10_ISR(void); = The switch statement waits until the voltage value detected by the ADC pin is within the detectable voltage range of the current board, and then the first case-statement waits for an ADC12MEM0 interrupt (12-bit ADC for these two boards). The CPUOFF bit (or LPM0_bits and GIE) is cleared, allowing the program to wait for the next force-exit and leave the ADC interrupt. Yet the ADC interrupt is not finished until all 4 if-statements are run through. Four if-statements determine which one of the 4 levels of brightness (determined by ranges of ADC12MEM0 values) is currently being detected by the ADC pin. The second case-statement waits for an ADC12MEM1 interrupt, representing the other ADC input pin. This case statement controls only the blue LED PWM output, while the first case statement controls green and red. The CPUOFF bit (or LPM0_bits and GIE) is cleared, allowing the program to wait for the next force-exit and leave the ADC interrupt. Four similar if-statements control the state of the other two GPIO white LEDs. The numbers themselves that are in the if-statement conditions come from wiring the photoresistor to the Photo-Resistors UART program and reading the maximum value obtained by the ADC, which over TX UART printed numbers around 60-65. The other two sensors, photodiode and phototransistor were roughly in the same range, (photodiode = 60-90, phototransistor 40-75). Thus, when the sensor detects full brightness at only 50 in ADC-value, both GPIO LEDs will turn off, and the RGB LED will be either orange, blue, or both (gray fuchsia) depending on which ADC pins are wired to sensors.
```

### Software Tools Used

* [Code Composer Studio](https://dev.ti.com/) - Code compiling program
