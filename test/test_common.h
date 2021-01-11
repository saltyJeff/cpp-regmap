#pragma once
#include <cstdint>

#include <regmap/regmap.h>
using namespace regmap;

/* Define test registers */
using ZERO_REG = Register<0, uint8_t>;
using ONE_REG = Register<1, uint8_t>;
using WORD_REG = Register<0x10, uint16_t>;

/* Define test register masks */
using WORD_BYTE_H = RegMask<WORD_REG, 15, 8>;
using WORD_BYTE_L = RegMask<WORD_REG, 7, 0>;

using MID_NIBBLE = RegMask<ONE_REG, 5, 2>;
using LOW_BIT = RegMask<ONE_REG, 0, 0>;
using HIGH_BIT = RegMask<ONE_REG, 7, 7>;

class DummyBus: public Bus {
public:
	int readAccesses = 0;
	int writeAccesses = 0;
	uint8_t mem[4] = {2, 4, 6, 8};
	int read(unsigned int deviceAddr, uint16_t regAddr, uint8_t *dest, unsigned int num) override {
		if(regAddr > sizeof(mem)) {
			return -1;
		}
		*dest = mem[regAddr];
		readAccesses++;
		return 0;
	}
	int write(unsigned int deviceAddr, uint16_t regAddr, uint8_t *src, unsigned int num) override {
		if(regAddr > sizeof(mem)) {
			return -1;
		}
		mem[regAddr] = *src;
		writeAccesses++;
		return 0;
	}
	~DummyBus() override = default;
};