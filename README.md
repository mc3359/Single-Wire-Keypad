# Single-Wire-Keypad
Single-wire 4x4 keypad with stable analog output (independent of pill contact resistance)

# Issues with conventional single-wire keypad
Single-wire keypad using MCU's analog converter to decode key press is not new. However, its reliability would degrade over time when the  membrane's pill contact has worn out or accumulated dirt. This causes the pill contact (mostly carbon-filled due to price) resistance to increase from few ohms to few kilo ohms! This huge change of membrane resistance could impact the accuracy of voltage level significantly since the values of biasing resistor matrix are few kilo-ohms to tens of kilo-ohms.

Example of conventional ADC-based keypad is here
https://www.edn.com/design/analog/4324528/Single-wire-keypad-interface-frees-microcontroller-I-O-pins

# Solution
Using a simple MCU with built-in R-2R DAC and analog op-amp as voltage buffer, like the PIC16F1704, to generate a stable analog voltage output. Detection of key press is by conventional matrix scanning method. 

The Arduino's keypad lib is ported to PIC16F for this implementation. MPLAB-X is the tool-chain.

# Description
1. This is based on Arduino's keypad library and ported to PIC16F1704
2. The keypad outputs both analog voltage and serial data.
3. Mapping of keys and analog values, and chars are defined in 
   
   key2dac_LUT[ROWS][COLS] and char keys[ROWS][COLS]
  
4. LED lits when there is a key press activity. If not it will gives heart-beat indication at 1sec rate
5. Supports hold-repeat key press @ 250ms rate (4 chars per sec)
6. This solution may replace your conventional single-wire keypad or prolong its life 
