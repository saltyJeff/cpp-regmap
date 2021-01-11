#include "test_common.h"
#include "doctest.h"

TEST_SUITE_BEGIN("regmap");

DummyBus bus;
Regmap<endian::big, ONE_REG> testMap(&bus, 0);

TEST_CASE("Reading and writing is correct") {
	uint8_t tmp;
	testMap.read<ZERO_REG>(tmp);
	CHECK(tmp == 2);
	testMap.write<ZERO_REG>(0x69);
	testMap.read<ZERO_REG>(tmp);
	CHECK(tmp == 0x69);
}

TEST_CASE("Bitmasking is applied") {
	uint8_t tmp;
	testMap.read<ZERO_REG>(tmp);
	uint8_t compare = applyMask<LOW_NIBBLE>(tmp, 3);
	testMap.write<LOW_NIBBLE>(3);
	testMap.read<ZERO_REG>(tmp);
	CHECK(tmp == compare);
}

TEST_CASE("Memoization occurs") {
	CHECK(testMap.isMemoized<ZERO_REG>() == false);
	CHECK(testMap.isMemoized<ONE_REG>() == true);
	int startReads = bus.readAccesses;
	uint8_t tmp;
	testMap.read<ONE_REG>(tmp);
	CHECK(bus.readAccesses - startReads == 1);
	testMap.read<ONE_REG>(tmp);
	CHECK(bus.readAccesses - startReads == 1);
	testMap.write<LOW_BIT>(5);
	CHECK(bus.readAccesses - startReads == 1);
}

TEST_SUITE_END();