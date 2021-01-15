#pragma once
#include <cstdint>
#include <type_traits>
#include "alufix_types.h"
/**
 * Utilities for working with different endian types
 */
namespace alufix {
	using alufix::types::endian;
	using alufix::types::ALUType;

	/**
	 * Swapping routines
	 */
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
	inline void* memcpy(void *dest, void* src, std::size_t n) {
		return __builtin_memcpy(dest, src, n);
	}
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
	inline void* mempcy(void *dest, void *src, std::size_t n) {
		for(int i = 0; i < n; i++) {
			dest[i] = src[i];
		}
		return dest;
	}
#endif
	template<endian ENDIANESS>
	inline void swapBytes(void *srcPtr, void* destPtr, std::size_t size) {
		if constexpr (ENDIANESS == endian::native) {
			memcpy(destPtr, srcPtr, size);
		}
		else {
			switch (size) {
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
	inline void toMachineRepresentation(void *aluPtr, uint8_t *machinePtr, std::size_t size) {
		if(size == 3) {
			uint32_t src = *static_cast<uint32_t*>(aluPtr);
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
	inline void toALURepresentation(uint8_t *aluPtr, uint8_t *machinePtr, std::size_t size) {
		if(size == 3) {
			uint32_t *alu = reinterpret_cast<uint32_t*>(aluPtr);
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
}