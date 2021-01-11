#pragma once
#include <cstdint>
#include <type_traits>
namespace regmap {
	// BITSET
	/**
	 * Get the required bytes needed to hold N bits
	 * @param n the number of bits to hold
	 * @return the number of bytes necessary
	 */
	constexpr unsigned int requiredBytes(unsigned int n) {
		return n % 8 == 0 ? n / 8 : n / 8 + 1;
	}
	template<unsigned int N>
	using bitset = uint8_t[requiredBytes(N)];

	inline bool bitset_test(uint8_t *bitset, unsigned int bit) {
		return bitset[bit / 8] & (1 << (bit % 8));
	}
	inline void bitset_set(uint8_t *bitset, unsigned  int bit) {
		bitset[bit / 8] |= 1 << (bit % 8);
	}
}