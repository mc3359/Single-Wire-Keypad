/* 
 * File:   emu_arduino.h
 * Author: vitco
 *
 * Created on August 20, 2019, 9:38 PM
 */

#ifndef EMU_ARDUINO_H
#define	EMU_ARDUINO_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "mytypedef.h"
// not used
typedef struct {
    union {
        struct {
         unsigned bit0: 1;  //lsb
         unsigned bit1: 1;
         unsigned bit2: 1;
         unsigned bit3: 1;
         unsigned bit4: 1;  
         unsigned bit5: 1;
         unsigned bit6: 1;
         unsigned bit7: 1;
         unsigned bit8: 1;
         unsigned bit9: 1;
         unsigned bit10: 1;
         unsigned bit11: 1;
         unsigned bit12: 1;
         unsigned bit13: 1;
         unsigned bit14: 1;
         unsigned bit15: 1; // msb
        } bits;
        u16 word16;  
    };
} reg16_t;    

    
#define LOW 0
#define HIGH 1    

// from wiring.h    
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))    
    
void pin_write(u8 pin_idx, bool st);
bool pin_read(u8 pin_idx);

//#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
//#define MICROSECONDS_PER_TIMER0_OVERFLOW (16000)  //16ms
#define MICROSECONDS_PER_TIMER0_OVERFLOW (2048)  //2.048ms, same as arduino
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)


extern volatile unsigned long timer0_overflow_count; 
extern volatile unsigned long timer0_millis;
unsigned long millis(void);
void delay_1ms(uint16_t d);

#ifdef	__cplusplus
}
#endif

#endif	/* EMU_ARDUINO_H */

