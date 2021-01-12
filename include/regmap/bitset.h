#pragma once
#include <cstdint>
#include <type_traits>
namespace regmap {
	/*
	 * A simple bitset implementation
	 */

	// use system integer size to reduce offset-addressing
	using REGMAP_BACKING_TYPE = int;

	static constexpr std::size_t INT_SIZE = sizeof(REGMAP_BACKING_TYPE);

	constexpr unsigned int requiredInts(unsigned int nBits) {
		return (nBits + INT_SIZE - 1) / INT_SIZE;
	}
	template<unsigned int N>
	using bitset = REGMAP_BACKING_TYPE[requiredInts(N)];

	inline bool bitset_test(const REGMAP_BACKING_TYPE *bitset, unsigned int bit) {
		return bitset[bit / INT_SIZE] & (1 << (bit % INT_SIZE));
	}
	inline void bitset_set(REGMAP_BACKING_TYPE *bitset, unsigned  int bit) {
		bitset[bit / INT_SIZE] |= 1 << (bit % INT_SIZE);
	}
}