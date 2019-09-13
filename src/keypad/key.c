// Ported from Arduino's keypad lib. Covert to C code (was Cpp)
//#include "mytypedef.h"
#include "key.h"

key_t key[10];

// methods
//void key();
//void key_user(char userKeyChar);
//void key_update(char userKeyChar, KeyState userState, bool userStatus);

// default constructor
void key_init(u8 i) {
	key[i].kchar = NO_KEY;
    key[i].kcode = -1;  // twt: added
	key[i].kstate = IDLE;
	key[i].stateChanged = false;
}

// constructor
void key_user_init(char userKeyChar, u8 i) {
	key[i].kchar = userKeyChar;
	key[i].kcode = -1;
	key[i].kstate = IDLE;
	key[i].stateChanged = false;
}

// not used
void key_update (char userKeyChar, KeyState userState, bool userStatus, u8 i) {
	key[i].kchar = userKeyChar;
	key[i].kstate = userState;
	key[i].stateChanged = userStatus;
}
