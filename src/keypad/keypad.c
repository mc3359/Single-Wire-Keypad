// Ported from Arduino's keypad lib. Covert to C code (was Cpp)
#include <stdio.h>
#include "..\emu_arduino.h"
#include "keypad.h"


// public
void keypad_init(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols);

//virtual void pin_mode(byte pinNum, byte mode) { pinMode(pinNum, mode); }
//virtual void pin_write(byte pinNum, boolean level) { digitalWrite(pinNum, level); }
//virtual int  pin_read(byte pinNum) { return digitalRead(pinNum); }

u16 bitMap[MAPSIZE];	// 10 row x 16 column array of bits. Except Due which has 32 columns.
//Key key[LIST_MAX];      // LIST_MAX = 10
unsigned long holdTimer;

// private
unsigned long startTime;
char *keymap;
byte *rowPins;
byte *columnPins;
KeypadSize sizeKpd;
u16 debounceTime;
u16 holdTime;
bool single_key;

void keypad_scanKeys(void);
bool keypad_updateList(void);
void keypad_nextKeyState(byte n, bool button);
void keypad_transitionTo(byte n, KeyState nextState);
void (*keypadEventListener)(char);

// <<constructor>> Allows custom keymap, pin configuration, and keypad sizes.
void keypad_init(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols) {
    u8 i;
    
	rowPins = row;
	columnPins = col;
	sizeKpd.rows = numRows;
	sizeKpd.columns = numCols;

	keypad_begin(userKeymap);

	// Scan rate of all keys are in one go (see keypad_scanKeys()...so each key is scanned at keypad_debounceTime!!! Not sequential!!!
    keypad_setDebounceTime(10);	// twt: org 10ms, define scanning rate. 50ms seems lesser double or more of same key where pressed
	keypad_setHoldTime(1000);	// twt: org 500ms. define key pressed and hold time (can be used for repeat key press)
	keypadEventListener = 0;

	startTime = 0;
	single_key = false;
    
    for (i=0;i<LIST_MAX;i++) key_init(i);   // init key state
}

// Let the user define a keymap - assume the same row/column count as defined in constructor
void keypad_begin(char *userKeymap) {
    keymap = userKeymap;
}

// Returns a single key only. Retained for backwards compatibility.
// TWT: only output a key when there is an open to close transistion
char keypad_getKey(void) {
	single_key = true;

	if (keypad_getKeys() && key[0].stateChanged && (key[0].kstate==PRESSED))
		return key[0].kchar;        // twt: possible bug....single_key  value is not revert back to false???
	
	single_key = false;

	return NO_KEY;
}

// twt: added. continuous to return the key so long it is pressed or hold state (i.e keep pressing!!)
char keypad_getKey_transparent(key_t **p) {
	single_key = true;
    *p = &key[0];        // always return key[0] obj
	if (keypad_getKeys() && key[0].stateChanged && (key[0].kstate==PRESSED)) {
        single_key = false;
		return key[0].kchar;
    }
    single_key = false;
    return NO_KEY;
}

// Populate the key list.
bool keypad_getKeys(void) {
	bool keyActivity = false;
    //return keyActivity;
	// Limit how often the keypad is scanned. This makes the loop() run 10 times as fast.
	if ( (millis()-startTime)>debounceTime ) {
        //printf("time %lu\n",millis());
		startTime = millis();   // twt: move to here
        keypad_scanKeys();
		keyActivity = keypad_updateList();
		//startTime = millis();  // org
	}

	return keyActivity;
}

// Private : Hardware scan
void keypad_scanKeys(void) {
	// Re-intialize the row pins. Allows sharing these pins with other hardware.
	//for (byte r=0; r<sizeKpd.rows; r++) {
		//pin_mode(rowPins[r],INPUT_PULLUP);
	//}

	// bitMap stores ALL the keys that are being pressed.
#if 0 // org
	for (byte c=0; c<sizeKpd.columns; c++) {  // twt: col is o/p and row is input
		//pin_mode(columnPins[c],OUTPUT);
		pin_write(columnPins[c], LOW);		// Begin column pulse output. // twt: after low, shouldn't it to delay a while for voltage to stable first (stray cap)?? 
		for (byte r=0; r<sizeKpd.rows; r++) {
			bitWrite(bitMap[r], c, !pin_read(rowPins[r]));  // keypress is active low so invert to high.
		}
		// Set pin to high impedance input. Effectively ends column pulse.
		pin_write(columnPins[c],HIGH);
		//pin_mode(columnPins[c],INPUT);
	}
#endif
    // twt: put rows as output and cols as input (reversed as opposed with arduino)
	for (byte r=0; r<sizeKpd.rows; r++) {  // twt: row is o/p and col is input
		pin_write(rowPins[r], LOW);		// row excitation -ve pulse (output) // twt: after low, shouldn't it to delay a while for voltage to stable first (stray cap)?? 
		delay_1ms(2);   // twt: added but smay remove. not much an impact, because the rate is at 20ms. So if this time is no stable, next round will be stable!!!!
        for (byte c=0; c<sizeKpd.columns; c++) {
			bitWrite(bitMap[r], c, !pin_read(columnPins[c]));  // keypress is active low so invert to high.
		}
		// Set pin to high impedance input. Effectively ends column pulse.
		pin_write(rowPins[r], HIGH);
		//pin_mode(columnPins[c],INPUT);
	}
}

// Manage the list without rearranging the keys. Returns true if any keys on the list changed state.
bool keypad_updateList(void) {

	bool anyActivity = false;
    bool button;

	// Delete any IDLE keys
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].kstate==IDLE) {
			key[i].kchar = NO_KEY;
			key[i].kcode = -1;
			key[i].stateChanged = false;
		}
	}

	// Add new keys to empty slots in the key list.
	for (byte r=0; r<sizeKpd.rows; r++) {
		for (byte c=0; c<sizeKpd.columns; c++) {
			button = bitRead(bitMap[r],c);
			char keyChar = keymap[r * sizeKpd.columns + c];
			int keyCode = r * sizeKpd.columns + c;
			int idx = keypad_findInList_int(keyCode);
			// Key is already on the list so set its next state.
			if (idx > -1)	{ // keycode is active, transit to next key state (to check close, release etc )
                //printf("DB: key idx=%d, state=%d\n",idx, key[idx].kstate);
				keypad_nextKeyState(idx, button);
                //printf("DB: key idx=%d, state=%d, st_chg = %d, keycode=%d, \n", 
                //idx, key[idx].kstate, key[idx].stateChanged, keyCode);
			}
			// A new key pressed is detected because keycode is NOT in the key list array (size 10, storing current active keys).
            // Add the keycode in key list and starts its FSM (i.e. activate FSM to detect close/open key)
			if ((idx == -1) && button) {
                //printf("DB: new key %c\n", keyChar);
				for (byte i=0; i<LIST_MAX; i++) {
					if (key[i].kchar==NO_KEY) {		// Find an empty slot or don't add key to list.
						key[i].kchar = keyChar;
						key[i].kcode = keyCode;
						key[i].kstate = IDLE;		// Keys NOT on the list have an initial state of IDLE.
						keypad_nextKeyState (i, button);
						break;	// Don't fill all the empty slots with the same key.
					}
				}
			}
		}
	}

	// Report if the user changed the state of any key.
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].stateChanged) anyActivity = true;
	}

	return anyActivity;
}

// Private
// This function is a state machine but is also used for debouncing the keys.
void keypad_nextKeyState(byte idx, bool button) {
	key[idx].stateChanged = false;

	switch (key[idx].kstate) {
		case IDLE:
			if (button==SW_CLOSED) {
				keypad_transitionTo(idx, PRESSED);
				holdTimer = millis(); }		// Get ready for next HOLD state.
			break;
		case PRESSED:
			if ((millis()-holdTimer)>holdTime)	// Waiting for a key HOLD...
				keypad_transitionTo(idx, HOLD);
			else if (button==SW_OPEN)				// or for a key to be RELEASED.
				keypad_transitionTo(idx, RELEASED);
			break;
		case HOLD:
			if (button==SW_OPEN)
				keypad_transitionTo(idx, RELEASED);
			break;
		case RELEASED:
			keypad_transitionTo(idx, IDLE);
			break;
	}
}

// New in 2.1
bool keypad_isPressed(char keyChar) {   // twt: not used
	for (byte i=0; i<LIST_MAX; i++) {
		if ( key[i].kchar == keyChar ) {
			if ( (key[i].kstate == PRESSED) && key[i].stateChanged )
				return true;
		}
	}
	return false;	// Not pressed.
}

// Search by character for a key in the list of active keys.
// Returns -1 if not found or the index into the list of active keys.
int keypad_findInList_char (char keyChar) {  // twt: not used
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].kchar == keyChar) {
			return i;
		}
	}
	return -1;
}

// Search by code for a key in the list of active keys.
// Returns -1 if not found or the index into the list of active keys.
int keypad_findInList_int (int keyCode) {
	for (byte i=0; i<LIST_MAX; i++) {
		if (key[i].kcode == keyCode) {
			return i;
		}
	}
	return -1;
}

// New in 2.0
char keypad_waitForKey(void) {  // twt: not used
	char waitKey = NO_KEY;
	while( (waitKey = keypad_getKey()) == NO_KEY );	// Block everything while waiting for a keypress.
	return waitKey;
}

// Backwards compatibility function.
KeyState keypad_getState(void) {
	return key[0].kstate;
}

// The end user can test for any changes in state before deciding
// if any variables, etc. needs to be updated in their code.
bool keypad_keyStateChanged(void) {
	return key[0].stateChanged;
}

// The number of keys on the key list, key[LIST_MAX], equals the number
// of bytes in the key list divided by the number of bytes in a Key object.
byte keypad_numKeys(void) {
	return (sizeof(key[0])/sizeof(key_t));
}

// Minimum debounceTime is 1 mS. Any lower *will* slow down the loop().
void keypad_setDebounceTime(u16 debounce) {
	if (debounce<1) debounceTime=1; else debounceTime=debounce;
}

void keypad_setHoldTime(u16 hold) {
    holdTime = hold;
}

void keypad_addEventListener(void (*listener)(char)){
	keypadEventListener = listener;
}

void keypad_transitionTo(byte idx, KeyState nextState) {
	key[idx].kstate = nextState;
	key[idx].stateChanged = true;

	// Sketch used the getKey() function.
	// Calls keypadEventListener only when the first key in slot 0 changes state.
	if (single_key)  {  // twt: listener always null...so will not call indirect function
	  	if ( (keypadEventListener!=0) && (idx==0) )  {
			keypadEventListener(key[0].kchar);
		}
	}
	// Sketch used the getKeys() function.
	// Calls keypadEventListener on any key that changes state.
	else {
	  	if (keypadEventListener!=0)  {
			keypadEventListener(key[idx].kchar);
		}
	}
}
