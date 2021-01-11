#pragma once
#include <cstdint>
#include <type_traits>
/**
 * Utilities for working with different endian types
 */
namespace endianfix {
	/**
	 * Comment this out as soon as C++20's new endian stuff is stable:
	 * 	https://en.cppreference.com/w/cpp/types/endian
	 */
	enum class endian {
	#ifdef _WIN32
		little = 0,
		big = 1,
		native = little
	#else
		little = __ORDER_LITTLE_ENDIAN__,
		big    = __ORDER_BIG_ENDIAN__,
		native = __BYTE_ORDER__
	#endif
	};

	/**
	 * Swapping routines
	 */
#ifdef __GNUC__
#define bswap16 __builtin_bswap16
#define bswap32 __builtin_bswap32
#define bswap64 __builtin_bswap64
#else
	inline uint16_t bswap16(uint16_t s) {
		return ((s & 0xFF) << 8) | s >> 8;
	}
	inline uint32_t bswap32(uint32_t i) {
		return (bswap16((i & 0xffff)) << 16) | bswap16(i >> 16);
	}
	inline uint64_t bswap64(uint64_t l) {
		return (bswap32((l & 0xffffffff) << 32)) | bswap32(l >> 32);
	}
#endif
	/**
	 * Definition for 24-bit integers
	 */
	using uint24_t = uint8_t[3];

	/**
	 * We abuse template specialization in order to provide a transparent conversion function
	 */
	// simplification to reduce the width of text lines
    template<bool CONDITION, typename RETURN_TYPE>
    using typeif = std::enable_if_t<CONDITION, RETURN_TYPE>;
    template<typename A, typename B>
    using equal = typename std::is_same<A, B>::value;

	// specialization: bytes do not get modified
	template<endian ENDIANESS>
	inline uint8_t fixEndianess(uint8_t *c) { return *c; }
	// specialization: only swap bytes if the endianness is not the same as native
	// for 16 bits
	template<endian ENDIANESS>
	inline typeif<ENDIANESS == endian::native, uint16_t> fixEndianess(uint16_t *s) { return *s; }
	template<endian ENDIANESS>
	inline typeif<ENDIANESS != endian::native, uint16_t> fixEndianess(uint16_t *s) { return bswap16(*s); }
	// for 32 bits
	template<endian ENDIANESS>
	inline typeif<ENDIANESS == endian::native, uint32_t> fixEndianess(uint32_t *i) { return *i; }
	template<endian ENDIANESS>
	inline typeif<ENDIANESS != endian::native, uint32_t> fixEndianess(uint32_t *i) { return bswap32(*i); }
	// for 64 bits
	template<endian ENDIANESS>
	inline typeif<ENDIANESS == endian::native, uint64_t> fixEndianess(uint64_t *l) { return *l; }
	template<endian ENDIANESS>
	inline typeif<ENDIANESS != endian::native, uint64_t> fixEndianess(uint64_t *l) { return bswap64(*l); }
	// specialization: no swap magic for the 24bit numbers
//	template<endian ENDIANESS>
//	inline typeif<ENDIANESS == endian::little, uint32_t> fixEndianess(uint24_t w) {
//		return w[0] + (w[1] << 8) + (w[2] << 16);
//	}
//	template<endian ENDIANESS>
//	inline typeif<ENDIANESS == endian::big, uint32_t> fixEndianess(uint24_t w) {
//		return (w[2] << 16) + (w[1] << 8) + w[0];
//	}
};