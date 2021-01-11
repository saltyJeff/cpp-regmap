#include "test_common.h"
#include "doctest.h"

DummyBus bus;
Regmap<endian::big, ONE_REG> testMap(std::shared_ptr<Bus>(&bus), 0);

TEST_CASE("Reading and writing is correct") {
	uint8_t tmp;
	testMap.read<ZERO_REG>(tmp);
	CHECK(tmp == 2);
	tmp = 4;
	testMap.write<ZERO_REG>(4);
	tmp = 0;
	testMap.read<ZERO_REG>(tmp);
	CHECK(tmp == 4);
}

TEST_CASE("Memoization occurs") {
	CHECK(testMap.isMemoized<ZERO_REG>() == false);
	CHECK(testMap.isMemoized<ONE_REG>() == true);
	//testMap.read<ZERO_REG>();
}