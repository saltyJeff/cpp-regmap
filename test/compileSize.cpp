#include "test_common.h"
#include <cstdio>
/**
 * A small function to check what exactly gets compiled out
 */
int main() {
	DummyBus bus;
	Regmap<endian::big, ONE_REG> testMap(&bus, 0);

	uint8_t x;
	testMap.read<ONE_REG>(x);

	testMap.write<WORD_BYTE_H, WORD_BYTE_L>(2, 6);

	// print all the variables we used, so the compiler can't optimize them out
	printf("screw u optimizing compiler: %d\n", x);
	return 0;
}