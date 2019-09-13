/* 
 * File:   defs_phk.h
 * Author: vitco
 *
 * Created on August 19, 2019, 10:52 AM
 */

/*
 
 Release history:
 
 V1.0-B0819
    - initial release
      - DAC output in transparent mode (once and holding key are handled in phe fw)
      - UART output supports once and holding mode
      - LED_STA: blinking in standy, lits when a key is pressed
      - WDT @ 2sec
 */

#ifndef DEFS_PHK_H
#define	DEFS_PHK_H

#ifdef	__cplusplus
extern "C" {
#endif
    
// === Feature switches ==
#define KEYMAP_LEGACY_VOUT      // compatible with old VCP firmware (but not GP 1X4 keypad)
//#define KEYMAP_NEW_VOUT         // remap the vout values for more reliable detection of key. Required PHE fw change. GP and VCP.
// ===
    
// inputs                       // arduino digital pin number
#define COL1_IN PORTCbits.RC3   // 0
#define COL2_IN PORTAbits.RA3   // 1    // MCLR pin, input only
#define COL3_IN PORTAbits.RA4   // 2
#define COL4_IN PORTAbits.RA5   // 3
#define COL5_IN PORTCbits.RC1   // 4 shared with LED_STA

// outputs
#define ROL1_OUT LATAbits.LATA0 // 5
#define ROL2_OUT LATAbits.LATA1 // 6 
#define ROL3_OUT LATAbits.LATA2 // 7
#define ROL4_OUT LATCbits.LATC0 // 8
#define LED_STA LATCbits.LATC1   

// emulate arduino pin read/write
// digital write
#define mDIG0_WRITE(st) (LATCbits.LATC3 = st)
#define mDIG1_WRITE(st) //(LATAbits.LATA3 = st)     // LATA3 is MCLR, input only!!!
#define mDIG2_WRITE(st) (LATAbits.LATA4 = st)
#define mDIG3_WRITE(st) (LATAbits.LATA5 = st)
#define mDIG4_WRITE(st) (LATCbits.LATC1 = st)
#define mDIG5_WRITE(st) (LATAbits.LATA0 = st)
#define mDIG6_WRITE(st) (LATAbits.LATA1 = st)
#define mDIG7_WRITE(st) (LATAbits.LATA2 = st)
#define mDIG8_WRITE(st) (LATCbits.LATC0 = st)
// digital read
#define mDIG0_READ  PORTCbits.RC3
#define mDIG1_READ  PORTAbits.RA3
#define mDIG2_READ  PORTAbits.RA4
#define mDIG3_READ  PORTAbits.RA5
#define mDIG4_READ  PORTCbits.RC1
#define mDIG5_READ  PORTAbits.RA0
#define mDIG6_READ  PORTAbits.RA1
#define mDIG7_READ  PORTAbits.RA2
#define mDIG8_READ  PORTCbits.RC0
    
#ifdef	__cplusplus
}
#endif

#endif	/* DEFS_PHK_H */

