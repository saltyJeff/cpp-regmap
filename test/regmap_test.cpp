#include "test_common.h"
#include "doctest.h"

TEST_SUITE_BEGIN("regmap");

TestRegmap testMap;

TEST_CASE("Reading and writing is correct") {
	uint8_t tmp;
	testMap.read<ZERO_REG>(tmp);
	CHECK(tmp == 2);
	testMap.write<ZERO_REG>(0x69);
	testMap.read<ZERO_REG>(tmp);
	CHECK(tmp == 0x69);
}

TEST_CASE("Zero-size regs work") {
	testMap.write<EMPTY_REG>();
}

TEST_CASE("Bitmasking is applied") {
	uint8_t tmp;
	testMap.read<ZERO_REG>(tmp);
	uint8_t compare = applyMask<LOW_NIBBLE>(tmp, 3);
	testMap.write<LOW_NIBBLE>(3);
	testMap.read<ZERO_REG>(tmp);
	CHECK(tmp == compare);
}

TEST_CASE("Mask merging") {
	uint8_t value = 0x55;
	testMap.write<ZERO_REG>(value);
	uint8_t writes = testMap.bus.writeAccesses;
	testMap.write<HIGH_NIBBLE, LOW_NIBBLE>(0x2, 0x1);
	CHECK(testMap.bus.writeAccesses == writes + 1);
	testMap.read<ZERO_REG>(value);
	CHECK(value == 0x21);

	uint8_t low, high;
	testMap.read<LOW_NIBBLE, HIGH_NIBBLE>(low, high);
	CHECK(low == 0x1);
	CHECK(high == 0x2);
}

TEST_CASE("Memoization occurs") {
	CHECK(testMap.isMemoized<ZERO_REG>() == false);
	CHECK(testMap.isMemoized<ONE_REG>() == true);
	int startReads = testMap.bus.readAccesses;
	uint8_t tmp;
	/* check for proper memoization of ONE_REG when:
	 * Reading the whole register for the first time
	 * Reading it again (should be served from memory)
	 * Reading a bitmask (should be masked-out from memory)
	 */
	testMap.read<ONE_REG>(tmp);
	CHECK(testMap.bus.readAccesses - startReads == 1);
	testMap.read<ONE_REG>(tmp);
	CHECK(testMap.bus.readAccesses - startReads == 1);
	testMap.write<LOW_BIT>(5);
	CHECK(testMap.bus.readAccesses - startReads == 1);

	/* check that writes are not memoized */
	int startWrites = testMap.bus.writeAccesses;
	testMap.write<ONE_REG>(5);
	CHECK(testMap.bus.writeAccesses - startWrites == 1);
}

TEST_CASE("Endianness is applied") {
	// device is in big-endian
	uint16_t value = 0x4732;
	testMap.write<WORD_REG>(value);

	// make sure masks still work in big-endian
	testMap.read<WORD_BYTE_H>(value);
	CHECK(value == 0x47);

	uint8_t msb;
	// the low byte should be the MSB
	testMap.read<Reg<0x10, uint8_t>>(msb);
	CHECK(msb == 0x47);
}

TEST_CASE("24-bit regs work") {
	uint32_t value = 0x123456;
	testMap.write<TWENTY_FOUR>(value);
	testMap.read<TWENTY_FOUR_HIGH>(value);
	CHECK(value == 0x12);

	testMap.write<TWENTY_FOUR_HIGH>(0x99);
	testMap.read<TWENTY_FOUR>(value);
	CHECK(value == 0x993456);
}

TEST_SUITE_END();