// Ported from Arduino's keypad lib. Covert to C code (was Cpp)

#ifndef KEYPAD_H
#define KEYPAD_H

#include "..\mytypedef.h"
#include "key.h"

#define SW_OPEN 0 //LOW
#define SW_CLOSED 1 //HIGH

typedef char KeypadEvent;
//typedef unsigned int uint;
//typedef unsigned long ulong;

// Made changes according to this post http://arduino.cc/forum/index.php?topic=58337.0
// by Nick Gammon. Thanks for the input Nick. It actually saved 78 bytes for me. :)
typedef struct {
    byte rows;
    byte columns;
} KeypadSize;

#define LIST_MAX 10		// Max number of keys on the active list. This is fixed at 10 in key.c
#define MAPSIZE 10		// MAPSIZE is the number of rows (times 16 columns)
#define makeKeymap(x) ((char*)x)      // convert x to 1d array by casting


//class Keypad : public Key, public HAL_obj {
// twt: Keypad inherits all methods and member variables from the class Key obj. 
//      The use of public means that the public methods and members of Key obj are also public in Keypad	

void keypad_init(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols);

char keypad_getKey(void);
bool keypad_getKeys(void);
KeyState keypad_getState();
void keypad_begin(char *userKeymap);
bool keypad_isPressed(char keyChar);
void keypad_setDebounceTime(u16);
void keypad_setHoldTime(u16);
void keypad_addEventListener(void (*listener)(char));
int keypad_findInList_char(char keyChar);
int keypad_findInList_int(int keyCode);
char keypad_waitForKey(void);
bool keypad_keyStateChanged(void);
byte keypad_numKeys(void);

char keypad_getKey_transparent(key_t **p);


#endif

