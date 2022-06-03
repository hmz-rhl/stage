// This example shows how you'd store an 8-byte structure with information about your board (that has the RTC on it)
// in protected block EEPROM. This block is hard to overwrite accidentally, so it's a good place to store this
// sort of info.
//
// You'd run code like this when you manufacture your board to store the identification information.
//
// The 8-board-id example shows how to read it.

// You should not normally define MCP79410_ENABLE_PROTECTED_WRITE. It allows writing to the protected block of EEPROM,
// a separate 8-byte EEPROM block. This is normally used for things like MAC addresses or board identification that you
// set once at the factory and do not update in the field. If used, define this before the first #include "MCP79410RK.h"
#define MCP79410_ENABLE_PROTECTED_WRITE

#include "MCP79410RK.h"

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler;
MCP79410 rtc;




void setup() {
	rtc.setup();
	
	
	rtc.eepromProtectedBlockWrite("aabbccdd");

	// Turn on the blue D7 LED so you know the data has been set
	//pinMode(D7, OUTPUT);
	//digitalWrite(D7, HIGH);
}

void loop() {
	rtc.loop();
}