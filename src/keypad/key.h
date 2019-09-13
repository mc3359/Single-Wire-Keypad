/* 
 * File:   key.h
 * Author: vitco
 *
 * Created on August 20, 2019, 2:41 PM
 */
// Ported from Arduino's keypad lib. Covert to C code (was Cpp)
#include "..\mytypedef.h"

#ifndef KEY_H
#define KEY_H

#define OPEN LOW
#define CLOSED HIGH

typedef unsigned int uint;
typedef enum{ IDLE, PRESSED, HOLD, RELEASED } KeyState;

const char NO_KEY = '\0';


typedef struct {
    char kchar;
    int kcode;
    KeyState kstate;
    bool stateChanged;
} key_t;

// methods
void key_init(u8 i);
void key_user_init(char userKeyChar, u8 i);
void key_update(char userKeyChar, KeyState userState, bool userStatus, u8 i);

extern key_t key[];

#endif

