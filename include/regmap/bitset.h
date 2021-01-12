#pragma once
#include <cstdint>
#include <type_traits>
namespace regmap {
	// a simple bitset implementation.

	// use system integer size to reduce offset-addressing
	static constexpr std::size_t INT_SIZE = sizeof(int);

	constexpr unsigned int requiredInts(unsigned int nBits) {
		return (nBits + INT_SIZE - 1) / INT_SIZE;
	}
	template<unsigned int N>
	using bitset = int[requiredInts(N)];

	inline bool bitset_test(const int *bitset, unsigned int bit) {
		return bitset[bit / INT_SIZE] & (1 << (bit % INT_SIZE));
	}
	inline void bitset_set(int *bitset, unsigned  int bit) {
		bitset[bit / INT_SIZE] |= 1 << (bit % INT_SIZE);
	}
}