#pragma once
#include <cstdint>
#include <regmap/regmap.h>
#include <cstring>

using namespace regmap;

/* Define test registers */
DECLR_BYTE(ZERO_REG, 0)
DECLR_BYTE(ONE_REG, 1)
DECLR_REG(WORD_REG, 0x10, uint16_t)

/* Define test register masks */
DECLR_MASK(WORD_BYTE_H, WORD_REG, 15, 8)
DECLR_MASK(WORD_BYTE_L, WORD_REG, 7, 0)

DECLR_MASK(LOW_NIBBLE, ZERO_REG, 3, 0)
DECLR_MASK(HIGH_NIBBLE, ZERO_REG, 7, 4)

DECLR_MASK(MID_NIBBLE, ONE_REG, 5, 2);
DECLR_MASK(LOW_BIT, ONE_REG, 0, 0);
DECLR_MASK(HIGH_BIT, ONE_REG, 7, 7);

class DummyBus: public Bus {
public:
	int readAccesses = 0;
	int writeAccesses = 0;
	uint8_t byteMem[4] = {2, 4, 6, 8};
	uint16_t wordMem[4] = {2, 4, 6, 8};

	constexpr uint8_t *resolveAddr(uint16_t regAddr) {
		if(0 <= regAddr && regAddr < 4) {
			return &byteMem[regAddr];
		}
		if(0x10 <= regAddr && regAddr < 0x18) {
			return ((uint8_t*)wordMem) + (regAddr - 0x10);
		}
		return nullptr;
	}

	int read(unsigned int deviceAddr, uint16_t regAddr, uint8_t *dest, unsigned int num) override {
		uint8_t *addr = resolveAddr(regAddr);
		if(addr == nullptr) {
			return -1;
		}
		memcpy(dest, addr, num);
		readAccesses++;
		return 0;
	}
	int write(unsigned int deviceAddr, uint16_t regAddr, uint8_t *src, unsigned int num) override {
		uint8_t *addr = resolveAddr(regAddr);
		if(addr == nullptr) {
			return -1;
		}
		memcpy(addr, src, num);
		writeAccesses++;
		return 0;
	}
	~DummyBus() override = default;
};