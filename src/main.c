/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC16F1704
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include <stdio.h>
#include "defs_phk.h"
#include "emu_arduino.h"
#include "./keypad/keypad.h"

// keypad config 
//const byte cROWS = 4; //four rows  // variable array length not supported in C, only for sC++
//const byte cCOLS = 3; //three columns
#define ROWS 4
#define COLS 4
char keys[ROWS][COLS] = {
  {'1','2','3','M'},
  {'4','5','6','A'},
  {'7','8','9','U'},
  {'*','0','#','D'}
};

#if 0
// Below keymap table (ADC value to key) is for reference only. It is defined in PHE key.c file
// [row][col] - 2nd 1.8 Beta Release (late July, for Sentosa proj) // for 10-bit ADC (PIC24FJ128GA106)
// compare sequence (from high val to low val): R1C4..R4C4 -> R1C3..R4C3 -> R1C2..R4C2 -> R1C1..R4C4 
const unsigned KP_VOUT_MIN[4][4] = {        //      R1..R4      // step size between rows with a fixed col, normalized
    {0xC0,0x5F,0x3F,**0x2E},                // C1: 1/5..1/2     // 0.050    0.083   0.1667      // e.g.: (1/4 - 1/5) = 0.05
    {0xF1,0x6A,0x44,**0x31},                // C2: 1/10..1/7    // 0.0111   0.0139  0.0179
    {0x141,0x77,*0x49,*0x34},               // C3: 1/15..1/12   // 0.0048   0.0055  0.0064 
    {0x1E1,0x88,0x50,0x37}};                // C4: 1/25..1/22   // 0.0017   0.0018  0.0020      // resolution 8b ADC 1/256 = 0.0039!!!


The follow changes (4 values) are needed on phe firmware (key.c of phe) const unsigned KP_VOUT_MIN[4][4]

* (may change to inc error margin for reliability...but so far so good) // for VCP and guard phone
0x49 -> 0x4A    // '9', bec DAC for '6' is 0x48...which is too close to 0x49!!!

** (MUST change)// for guard phone
0x2E -> 0x2B    // 'M'  // MUST change   
0x31 -> 0x2F    // 'A'  // MUST change  
0x34 -> 0x33    // 'U'  // may change for reliability

phk: keypad board, phe: telephony vcp board

Note : per step of DAC (8b, phk) gives 4 counts of ADC (10bit, phe) value

Actions need to use phk board
-----------------------------        
- for VCP, temporary no changes on phe firmware to use phk board
- for GP, must change the KP_VOUT_MIN[][] LUT in order to use phk board!!!!
- For best result, use the UART TX interface instead. 
  - PHE may use bit-bang method for software serial 
   (see arduino lib for example...edge-trg interrupt (with hw counter??) driven, not sampling driven, so it is faster. Claim to support > 9600)
  - OR share UART RX pin (RP29) with debug UART (com4 in fw but com2 in sch) by redirecting its UART RX to RB1 (RP1, KB_VOUT) in operational  mode. 
    When test mode is activated, the RX is at the default pin (RP29), but only switch this pin to RP1 when testing keypad...  ## use this method for UART!! ##  
    The phk buad is set to 38K4 for compability with phe debug port.
            
#endif

#ifdef KEYMAP_LEGACY_VOUT             
u8 key2dac_LUT[ROWS][COLS] = {      // actual voltage read out by PHE (10-bit)
    {51,26,17-1,10+1},              //  0xCC    0x68    0x40    0x2C
    {64,28,18,11+1},                //  0x100   0x70    0x48    0x30    
    {85,32,20-1,12+1},              //  0x154   0x80    0x4C    0x34
    {128,37,21,13+2}                //  0x200   0x94    0x54    0x38
#endif

// Proposed table (phe VCP also needs change) to increase gap of DAC by at least 2 counts. 
// Especially COL3 and COL4: '3','6','9', '#', 'M', 'A' etc...  
#ifdef KEYMAP_NEW_VOUT              // expected voltage read out by PHE (10-bit) <== need PHE fw change. VMIN should set to 0x20 (now 0x26)
u8 key2dac_LUT[ROWS][COLS] = {      // new threshold for COL3 and COL4. The rest reminds      
    {51,26,16,8},                   //  0xCC    0x68    0x40    0x20      // 0x44    0x24
    {64,28,18,10},                  //  0x100   0x70    0x48    0x28      // 0x4C    0x2C
    {85,32,20,12},                  //  0x154   0x80    0x50    0x30      // 0x54    0x34
    {128,37,22,14}                  //  0x200   0x94    0x58    0x38      // 0x5C    0x3C
#endif
    


#if 0   // for testing col4 val (guardphone) but shift to col3. Using 4x3 keypad to test
    {51,26,10+1,10},
    {64,28,11+1,11},
    {85,32,12+1,12},
    {128,37,13+1,13}    
#endif
};
        
byte myColPins[COLS] = {0, 1, 2, 3}; //connect to the column pinouts of the keypad  // output (excitation), pin4 used for LED_STA
byte myRowPins[ROWS] = {5, 6, 7, 8}; //connect to the row pinouts of the keypad     // input (detect)
char mykey;
key_t *myKeyObj_ptr;

u8 *key2dac_lut_ptr;
unsigned long repeatTime;
u32 heartBeatTime;
/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    keypad_init(makeKeymap(keys), myRowPins, myColPins, ROWS, COLS );
    
    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    // twt: enable interrupt here
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    
    // set RC1 as output (default input) for LED
    TRISCbits.TRISC1 = 0;   // for LED_STA
    key2dac_lut_ptr = (u8*)key2dac_LUT;   // convert 2D to 1D array
    heartBeatTime = millis();
    printf("keypad is ready..\r");    // cannot print numeric number as this may interfere with keypad char
    while (1)
    {

        //mykey = keypad_getKey();
        //if (mykey != NO_KEY){
        //    printf("%c\r",mykey);
        //}

        mykey = keypad_getKey_transparent(&myKeyObj_ptr);
        if (myKeyObj_ptr->kchar != NO_KEY) {        // key is press (transparent mode..like transparent logic!!)
            LED_STA = 0;
            DAC_SetOutput(key2dac_lut_ptr[myKeyObj_ptr->kcode]);
            if (mykey != NO_KEY) {                  // ensure only one char is send for initial key press
                printf("%c\r",myKeyObj_ptr->kchar);
                repeatTime = millis();
            } else if (myKeyObj_ptr->kstate==HOLD) {
              	if ((millis()-repeatTime)>250) {     // repeat char output every 250ms
                    repeatTime = millis();
                    printf("%c\r",myKeyObj_ptr->kchar);
                }
            }
        } else {
             DAC_SetOutput(0);
#ifdef KEYMAP_LEGACY_VOUT 
             if ((millis()-heartBeatTime)>500) {
#else
             if ((millis()-heartBeatTime)>100) {
#endif
                heartBeatTime = millis();
                LED_STA ^= 1;
             }
        }
        CLRWDT();   // wdt time is set to ~2sec in MCC
    }
}


// TWT: define putch to support printf()
/*****************************
Dependencies:   xc.h, stdio.h
Processor:      PIC16F1824
Complier:       XC8 v1.00 or higher 
Result: Prints to the UART1 Window of the IDE
Debugger: Simulator
*****************************/
void putch(unsigned char byte)
{
    TXSTA=0x26;
    RCSTAbits.SPEN=1;
    TXREG=byte;
    while(!TXIF)continue;
    TXIF=0;
}
/**
 End of File
*/