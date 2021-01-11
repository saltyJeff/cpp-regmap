#include "doctest.h"
#include "test_common.h"

TEST_SUITE_BEGIN("Utilities");

TEST_CASE("Bitmasks work correctly") {
	SUBCASE("Shifting in values works") {
		CHECK(shiftInValue<WORD_BYTE_H>(0x69) == 0x6900);
		CHECK(shiftInValue<WORD_BYTE_L>(0x69) == 0x0069);
	}
	SUBCASE("Shifting out values works") {
		CHECK(shiftOutValue<WORD_BYTE_H>(0x6900) == 0x69);
		CHECK(shiftOutValue<WORD_BYTE_H>(0x0069) == 0x00);
		CHECK(shiftInValue<WORD_BYTE_L>(0x6900) == 0x00);
		CHECK(shiftInValue<WORD_BYTE_L>(0x0069) == 0x69);
	}
	SUBCASE("Applying masks to existing values works") {
		CHECK(applyMask<LOW_NIBBLE>(0x66, 0x9) == 0x69);
		CHECK(applyMask<HIGH_BIT>(0xAA, 0) == 0x2A);
	}
	SUBCASE("Merging masks is correct") {
		using HIGH_BYTE_12 = MaskVal<WORD_BYTE_H, 0x12>;
		using LOW_BYTE_21 = MaskVal<WORD_BYTE_L, 0x21>;
		using mergeResult = MergeMaskVals<HIGH_BYTE_12, LOW_BYTE_21>;
		CHECK(mergeResult::val == 0x1221);

		using HIGH_BIT_1 = MaskVal<HIGH_BIT, 1>;
		using LOW_BIT_1 = MaskVal<LOW_BIT, 1>;
		using MID_NIBBLE_2 = MaskVal<MID_NIBBLE, 2>;
		using mergeResult1 = MergeMaskVals<HIGH_BIT_1, LOW_BIT_1, MID_NIBBLE_2>;
		CHECK(mergeResult1::val == 0b10001001);
	}
}
TEST_CASE("Bitsets work correctly") {
	bitset<10> bits = {0};
	CHECK(bitset_test(bits, 3) == false);
	CHECK(bitset_test(bits, 8) == false);
	bitset_set(bits, 8);
	CHECK(bitset_test(bits, 8) == true);
}

TEST_SUITE_END();