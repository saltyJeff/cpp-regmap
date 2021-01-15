#include "doctest.h"
#include "test_common.h"

TEST_SUITE_BEGIN("Utilities");

TEST_CASE("Bitmasks work correctly") {
	SUBCASE("Masks are correct") {
		CHECK(bitmask<MID_NIBBLE>() == 0b00111100);
		CHECK(bitmask<HIGH_NIBBLE>() == 0xF0);
		CHECK(bitmask<HIGH_BIT>() == 0x80);
		CHECK(bitmask<LOW_BIT>() == 1);
	}
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
		// check value implementation
		auto mergeResult = mergeMasks<WORD_BYTE_H, WORD_BYTE_L>(0x12, 0x21);
		CHECK(mergeResult == 0x1221);

		// check type implementation
		using MaskMerged = MergeMasks<WORD_BYTE_L, WORD_BYTE_H>;
		CHECK(MaskH<MaskMerged>() == 15);
		CHECK(MaskL<MaskMerged>() == 0);
		CHECK(MaskSpansRegister<MaskMerged>());
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