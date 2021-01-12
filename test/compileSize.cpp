#include "test_common.h"
#include <cstdio>
/**
 * A small function to make sure the compile size is OK
 */

DECLR_REG(WORD_1_REG, 0x11, uint16_t);
int main() {
	DummyBus bus;
	Regmap<endian::big, ONE_REG, WORD_1_REG> testMap(&bus, 0);

	uint8_t x;
	testMap.read<ONE_REG>(x);

	uint16_t a;
	testMap.read<WORD_BYTE_H>(a);

	// print all the variables we used, so the compiler can't optimize them out
	printf("screw u optimizing compiler: %x, %x, %x\n", x, a);

	printf("Sizeof memo: %d\n", sizeof(testMap.memoized));
	return 0;
}