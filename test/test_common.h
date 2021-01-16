#pragma once
#include <cstdint>
#include <regmap/regmap.h>
#include <cstring>

using namespace regmap;

DECLR_BYTE(ZERO_REG, 0)
DECLR_BYTE(ONE_REG, 1)
DECLR_CMD(EMPTY_REG, 2)
DECLR_REG(WORD_REG, 0x10, uint16_t)
DECLR_REG(TWENTY_FOUR, 0x24, uint24_t)

/* Define test register masks */
DECLR_MASK(WORD_BYTE_H, WORD_REG, 15, 8)
DECLR_MASK(WORD_BYTE_L, WORD_REG, 7, 0)

DECLR_MASK(LOW_NIBBLE, ZERO_REG, 3, 0)
DECLR_MASK(HIGH_NIBBLE, ZERO_REG, 7, 4)

DECLR_MASK(MID_NIBBLE, ONE_REG, 5, 2);
DECLR_MASK(LOW_BIT, ONE_REG, 0, 0);
DECLR_MASK(HIGH_BIT, ONE_REG, 7, 7);

DECLR_MASK(TWENTY_FOUR_HIGH, TWENTY_FOUR, 23, 16)

class DummyBus {
public:
	int readAccesses = 0;
	int writeAccesses = 0;
	uint8_t byteMem[4] = {2, 4, 6, 8};
	uint16_t wordMem[4] = {2, 4, 6, 8};
	uint24_t twentyFourMem;

	constexpr uint8_t *resolveAddr(uint8_t regAddr) {
		if(0 <= regAddr && regAddr < 4) {
			return &byteMem[regAddr];
		}
		if(0x10 <= regAddr && regAddr < 0x18) {
			return ((uint8_t*)wordMem) + (regAddr - 0x10);
		}
		if(regAddr == 0x24) {
			return (uint8_t*)twentyFourMem;
		}
		return nullptr;
	}

	int read(uint8_t regAddr, uint8_t *dest, uint8_t num) {
		uint8_t *reg = resolveAddr(regAddr);
		if(reg == nullptr) {
			return -1;
		}
		memcpy(dest, reg, num);
		readAccesses++;
		return 0;
	}
	int write(uint8_t regAddr, uint8_t *src, uint8_t num) {
		uint8_t *reg = resolveAddr(regAddr);
		if(reg == nullptr) {
			return -1;
		}
		memcpy(reg, src, num);
		writeAccesses++;
		return 0;
	}
	~DummyBus() = default;
};

class TestRegmap: public Regmap<endian::big, uint8_t, ONE_REG, TWENTY_FOUR> {
public:
	DummyBus bus;

	int deviceRead(uint8_t regAddr, uint8_t *src, uint8_t num) override {
		return bus.read(regAddr, src, num);
	}

	int deviceWrite(uint8_t regAddr, uint8_t *src, uint8_t num) override {
		return bus.write(regAddr, src, num);
	}
};