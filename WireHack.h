#ifndef WIREHACK_H
#define WIREHACK_H

#include <cstdint>

class WireHack
{
public:
	WireHack() {};
	void begin() {};
	void beginTransmission(uint8_t address) {};
	void write(uint8_t byte) {};
	void endTransmission() {};
	int requestFrom(uint8_t address, uint8_t reg) {return 0;};
	int available() {return 0;};
	uint8_t read() {return 0;};
};

void delay(uint8_t) {};

extern WireHack Wire;
#endif
