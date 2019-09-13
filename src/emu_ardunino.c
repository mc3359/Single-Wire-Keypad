#include "mcc_generated_files/mcc.h"
#include "mytypedef.h"
#include "defs_phk.h"
#include "./mcc_generated_files/tmr0.h"
#include "emu_arduino.h"




/*
// inputs                       // arduino pin number
#define COL1_IN PORTAbits.RA5   // 0
#define COL2_IN PORTAbits.RA4   // 1
#define COL3_IN PORTAbits.RA3   // 2
#define COL4_IN PORTCbits.RC3   // 3
#define COL5_IN PORTCbits.RC1   // 4 shared with LED_STA

// outputs
#define ROL1_OUT LATAbits.LATA0 // 5
#define ROL2_OUT LATAbits.LATA1 // 6 
#define ROL3_OUT LATAbits.LATA2 // 7
#define ROL4_OUT LATCbits.LATC0 // 8
*/

// emulate arduino pin_write() and pin read() functions (digitalRead and digitalWrite)
void pin_write(u8 pin_idx, bool st)
{
    switch (pin_idx) {
        case 0:
            mDIG0_WRITE(st);
            break;
        case 1:
            mDIG1_WRITE(st);
            break;
        case 2:
            mDIG2_WRITE(st);
            break;
        case 3:
            mDIG3_WRITE(st);        
            break;
        case 4:
            mDIG4_WRITE(st);
            break;
        case 5:
            mDIG5_WRITE(st);
            break;
        case 6:
            mDIG6_WRITE(st);
            break;
        case 7:
            mDIG7_WRITE(st);
            break;
        case 8:
            mDIG8_WRITE(st);
            break;
        default:    
            break;
    }
}

bool pin_read(u8 pin_idx)
{
    switch (pin_idx) {
        case 0:
            return(mDIG0_READ);
            break;
        case 1:
            return(mDIG1_READ);
            break;
        case 2:
            return(mDIG2_READ);
            break;
        case 3:
            return(mDIG3_READ);
            break;
        case 4:
            return(mDIG4_READ);
            break;
        case 5:
            return(mDIG5_READ);
            break;
        case 6:
            return(mDIG6_READ);
            break;
        case 7:
            return(mDIG7_READ);
            break;
        case 8:
            return(mDIG8_READ);
            break;
        default:
            return(0);
    }
}

unsigned long millis(void)
{
  u32 m;
  bool gie_bit = INTCONbits.GIE;
  // Note: in millis() function, cannot save the INTCON register and than restore back (like arduino code)...don't know why???
  //       So just save the GIE bit and then restore instead!!!
  //uint8_t oldINTCON = INTCON;     
  // disable interrupts while we read timer0_millis or we might get an
  // inconsistent value (e.g. in the middle of a write to timer0_millis)
  INTERRUPT_GlobalInterruptDisable();

  m = timer0_millis;
  
  //INTCON = oldINTCON;
  //INTERRUPT_GlobalInterruptEnable();
  INTCONbits.GIE = gie_bit;
  return m;
}


void delay_1ms(uint16_t d)
{
    uint8_t i;
    uint16_t i16;
    
    for(i16=0;i16<d;i16++)
        for(i=0;i<209;i++);
    
}
