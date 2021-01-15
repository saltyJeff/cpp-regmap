#pragma once
#include <cstdint>
#include <type_traits>
#include "alufix_types.h"
#include "utils.h"

namespace alufix::constSwaps {
	constexpr inline uint16_t bswap16(uint16_t s) {
		return ((s & 0xFF) << 8) | s >> 8;
	}
	constexpr inline uint32_t bswap32(uint32_t i) {
		return (bswap16((i & 0xffff)) << 16) | bswap16(i >> 16);
	}
	constexpr inline uint64_t bswap64(uint64_t l) {
		return (bswap32((l & 0xffffffff) << 32)) | bswap32(l >> 32);
	}
}

namespace alufix {
	using alufix::types::endian;
	using alufix::types::ALUType;

#ifdef __GNUC__
	inline uint16_t bswap16(uint16_t s) {
		return __builtin_bswap16(s);
	}

	inline uint32_t bswap32(uint32_t i) {
		return __builtin_bswap32(i);
	}

	inline uint64_t bswap64(uint64_t l) {
		return __builtin_bswap64(l);
	}

	inline void *memmove(void *dest, void *src, std::size_t n) {
		return __builtin_memmove(dest, src, n);
	}

	inline void *memcpy(void *dest, void *src, std::size_t n) {
		return __builtin_memcpy(dest, src, n);
	}
#else
	auto bswap16 = constSwaps::bswap16;
	auto bswap32 = constSwaps::bswap32;
	auto bswap64 = constSwaps::bswap64;

	inline void* memcpy(void *dest, void *src, std::size_t n) {
		auto *destPtr = reinterpret_cast<uint8_t*>(dest);
		auto *srcPtr = reinterpret_cast<uint8_t*>(src);
		for(int i = 0; i < n; i++) {
			destPtr[i] = srcPtr[i];
		}
		return dest;
	}
	auto memmove = memcpy;
#endif

	/**
	 * Swapping routines
	 */

	template<endian ENDIANESS>
	inline void swapBytes(void *srcPtr, void* destPtr, std::size_t size) {
		if constexpr (ENDIANESS == endian::native) {
			if(srcPtr != destPtr) {
				// will break if pointers overlap. But then again, how did u get here
				memcpy(destPtr, srcPtr, size);
			}
		}
		else {
			switch (size) {
			case 1:
				*reinterpret_cast<uint8_t *>(destPtr) = *reinterpret_cast<uint8_t *>(srcPtr);
				break;
			case 2:
				*reinterpret_cast<uint16_t *>(destPtr) = bswap16(*static_cast<uint16_t *>(srcPtr));
				break;
			case 4:
				*reinterpret_cast<uint32_t *>(destPtr) = bswap32(*static_cast<uint32_t *>(srcPtr));
				break;
			case 8:
				*reinterpret_cast<uint64_t *>(destPtr) = bswap64(*static_cast<uint64_t *>(srcPtr));
			default:
				break;
			}
		}
	}

	/**
	 * Copies an integer from its ALU representation to
	 * a machine representation
	 * @tparam ENDIANESS the endianness of the machine
	 * @param aluPtr the source ALU representation
	 * @param machinePtr the machine destination
	 * @param size the size of the integer
	 */
	template<endian ENDIANESS>
	inline void toDeviceFormat(void *aluPtr, uint8_t *machinePtr, std::size_t size) {
		if(size == 3) {
			auto src = *static_cast<uint32_t*>(aluPtr);
			if constexpr (ENDIANESS == endian::big) {
				machinePtr[0] = (src >> 16) & 0xFF;
				machinePtr[1] = (src >> 8) & 0xFF;
				machinePtr[2] = src & 0xFF;
			}
			else {
				machinePtr[2] = (src >> 16) & 0xFF;
				machinePtr[1] = (src >> 8) & 0xFF;
				machinePtr[0] = src & 0xFF;
			}
		}
		else {
			swapBytes<ENDIANESS>(aluPtr, machinePtr, size);
		}
	}

	/**
	 * Copies an integer from its machine representation to
	 * its ALU representation
	 * @tparam ENDIANESS the endianess of the machine
	 * @param aluPtr the destination ALU representation
	 * @param machinePtr the machine source
	 * @param size the size of the integer
	 */
	template<endian ENDIANESS>
	inline void toLocalALUFormat(uint8_t *aluPtr, uint8_t *machinePtr, std::size_t size) {
		if(size == 3) {
			auto alu = reinterpret_cast<uint32_t*>(aluPtr);
			if constexpr (ENDIANESS == endian::big) {
				*alu = (machinePtr[0] << 16) + (machinePtr[1] << 8) + machinePtr[2];
			}
			else {
				*alu = (machinePtr[2] << 16) + (machinePtr[1] << 8) + machinePtr[0];
			}
		}
		else {
			swapBytes<ENDIANESS>(machinePtr, aluPtr, size);
		}
	}
	/**
	 * Fixes the endianness of a value at compile time
	 * @tparam ENDIANNESS the desired endianness
	 * @tparam N The width of the integer in bytes
	 * @param value The value you want to fix
	 */
	template<endian ENDIANNESS, typename T>
	inline void fixEndianness(T &value) {
		constexpr std::size_t N = sizeof(T);
		static_assert(N == 1 || N == 2 || N == 4 || N == 8,
			"Register addresses must be 1,2,4, or 8 bytes wide");
		if constexpr (ENDIANNESS == endian::native) {
			return;
		}
		else if constexpr (N == 1) {
			return;
		}
		else if constexpr (N == 2) {
			value = constSwaps::bswap16(value);
		}
		else if constexpr (N == 4) {
			value = constSwaps::bswap32(value);
		}
		else if constexpr (N == 8) {
			value = constSwaps::bswap64;
		}
	}
}