# PWM RGB LED

The provided main.c code files for the MSP430G2553, the MSP430FR6989, and the MSP430F5529 are designed to measure the voltage value (between roughly +0.5V and +3V) going into the ADC pin, and convert the measured ADC value into three different PWM outputs for illuminating an RGB LED. The ADC pin is wired to any of the three photo-sensors; no additional conversion/amplification is needed since the circuits for each photo-sensor amplify the voltage ranges to be in the same range as the photoresistor. When the sensor detects mostly darkness the RGB LED illuminates a dark brown, and when the sensor detects mostly brightness the RGB LED illuminates a bright orange. There are also two GPIO LED output pins placed in the code as well; these simulate night-light lights. The two white GPIO LEDs indicate in binary which level of brightness is currently being detected (4 levels: bright 00, somewhat bright 01, somewhat dark 10, dark 11; in binary). 

### Installing on MSP430G2553

Only jumper wires are required to wire the photoresistor (or other sensor) circuit to the ADC pin and to wire the RGB LED to the three PWM output pins. The longest RGB LED wire is wired to GND. Jumper wires are also needed to wire two white LEDs to the GPIO output pins.

Pins used:

```
P1.3 = ADC pin
P1.2 = PWM output to red LED on RGB LED
P2.2 = PWM output to green LED on RGB LED
P2.4 = PWM output to blue LED on RGB LED
P2.0 = GPIO white LED
P2.1 = GPIO white LED
```

### Installing on MSP430FR6989

Only jumper wires are required to wire the photoresistor (or other sensor) circuit to the ADC pin and to wire the RGB LED to the three PWM output pins. The longest RGB LED wire is wired to GND. Jumper wires are also needed to wire two white LEDs to the GPIO output pins.

Pins used:

```
P9.2 = ADC pin
P3.3 = PWM output to red LED on RGB LED
P2.6 = PWM output to green LED on RGB LED
P2.7 = PWM output to blue LED on RGB LED
P2.0 = GPIO white LED
P2.1 = GPIO white LED
```

### Installing on MSP430F5529

Only jumper wires are required to wire the photoresistor (or other sensor) circuit to the ADC pin and to wire the RGB LED to the three PWM output pins. The longest RGB LED wire is wired to GND. Jumper wires are also needed to wire two white LEDs to the GPIO output pins.

Pins used:

```
P6.0 = ADC pin
P2.0 = PWM output to red LED on RGB LED
P2.4 = PWM output to green LED on RGB LED
P2.5 = PWM output to blue LED on RGB LED
P1.2 = GPIO white LED
P1.3 = GPIO white LED
```

### Code-Breakdown

Below are concise descriptions for each of the listed "void" functions within the code (all three MSP430 boards):

```
int main(void); = The GPIO LED pins (including the 3 PWM output pins) are initialized, the 3 RGB PWM timers are initialized, and the ADC input pin is initialized. For the MSP430G2553 only, the code for controlling the 3 PWM duty cycles and the GPIO white LEDs is found within a for-loop in the main function block; for the other two boards, this code is found in the ADC interrupt block. Beginning in the for-loop, the ADC conversion is started and the ADC interrupt is force-exited. There is no while-loop or anything telling the MSP430G2553 to wait until conversion finishes, since any such lines causes the program to hang at that line indefinitely. Four if-statements determine which one of the 4 levels of brightness is currently being detected by the ADC pin. Regardless of the current if-statement brightness level, the PWM outputs (TxxCCRx, i.e. TA0CCR1) do not change or get divided differently in any of the modes. The green LED PWM output is divided by 3, and the blue LED PWM output is divided by 5 to illuminate the RGB LED with a bright orange color when detecting full brightness.
```

```
__interrupt void ADC10_ISR(void); = For the MSP430G2553, the CPUOFF bit is cleared, allowing the program to wait for the next force-exit line and leave the ADC interrupt. For the other two boards, all the code within the for-loop described above (excluding the actual for-loop) is found in the ADC interrupt and within a switch-statement. The switch statement waits until the voltage value detected by the ADC pin is within the detectable voltage range of the current board, and then a single case-statement waits for an ADC12MEM0 interrupt (12-bit ADC for these two boards). The CPUOFF bit (or LPM0_bits and GIE) is cleared, allowing the program to wait for the next force-exit and leave the ADC interrupt. Yet the ADC interrupt is not finished until all 4 if-statements are run through. Four if-statements determine which one of the 4 levels of brightness is currently being detected by the ADC pin. 
```

### Software Tools Used

* [Code Composer Studio](https://dev.ti.com/) - Code compiling program
