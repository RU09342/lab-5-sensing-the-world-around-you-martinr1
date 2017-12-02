# MSP430FR2311 16-pin PCB Schematic

This schematic interfaces with a 16-pin surface-mounted MSP430FR2311 microprocessor. The DVcc pin is wired to a bulk capacitor of 10 uF as well as a bypass capacitor of 100 nF. All capacitors are also wired to GND. The ~RST reset pin is typically wired to a button on the red developer's board, but as a stand-alone IC chip, a pull-up resistor and a bypass capacitor are needed to stabilize the inputs of the push-button that is wired to the header pin connecting to the ~RST pin. All other pins of the MSP430FR2311 16-pin IC are wired directly to through-hole headers for direct programming from the PC.

### Bill of Materials

* [Mouser Components](https://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=04844adf43) - Required components for PCB board, Mouser items cart.

# Photoresistor Amplifier PCB Schematic

The photoresistor schematic involves only the photoresistor itself and a 10K ohm resistor in series. In between these two discrete components is a wire that connects to the ADC pin of the MSP430 board (MSP430G2553). The photoresistor is connected directly to Vcc while the 10K ohm resistor is directly connected to GND.

### Bill of Materials

* [Mouser Components](https://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=3f99785d73) - Required components for PCB board, Mouser items cart.

# Phototransistor Amplifier PCB Schematic

The phototransistor implements the LM324 low-rail-voltage 4-channel operational amplifier. The phototransistor is identified as the OP805SL. A bulk capacitor of 1 uF is wired from +3.3V to GND before going to the adjacently connected phototransistor. The op-amp provides a 1x gain (no gain) with its two 100 Kohm resistors. After this op-amp, the output goes to a second op-amp that serves as an active low-pass filter (using the 100 Kohm resistor and the 1 uF capacitor). The output of this second op-amp goes directly to a header pin for wiring to an ADC pin of an MSP430 board (roughly the same voltage range as the photoresistor). 

### Bill of Materials

* [Mouser Components](https://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=1f1809b86a) - Required components for PCB board, Mouser items cart.

# Photodiode Interface PCB Schematic

The photodiode also implements the LM324 low-rail-voltage 4-channel operational amplifier. The photodiode is identified as the OP950. A bulk capacitor of 1 uF is wired from +3.3V to GND. The cathode side of the OP950 is wired to the +INC pin of the first op-amp, while the cathode is wiredt to the -INC pin and the corresponding active low-pass filter of the same op-amp (1 Mohm resistor and 2.7 pF capacitor). The output of this first op-amp goes into a 1x gain (no gain) op-amp that uses two 10 Kohm resistors and a 47 pF capacitor. The output of this second op-amp goes directly to a header pin for wiring to an ADC pin of an MSP430 board (roughly the same voltage range as the photoresistor). 

### Bill of Materials

* [Mouser Components](https://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=44b1d4a6e9) - Required components for PCB board, Mouser items cart.

### Software Tools Used

* [DipTrace](https://www.diptrace.com/) - Schematic editing and PCB editing program.
