#pragma once
#include "register.h"
#include "bus.h"
#include "bitstuff.h"
#include <memory>
#include <cstdint>
#include "endianfix.h"
//
//namespace regmap {
//	/**
//	 * An implementation of a register map.
//	 *
//	 * @tparam WORD_SZ the type of each word in the regmap
//	 * @tparam ENDIAN the endianness of the device
//	 * @tparam REGS registers to memoize. Make sure that there are no duplicates in this register!
//	 */
//	template<typename WORD_SZ,
//		endianfix::endian ENDIAN,
//		Register... REGS>
//	class Regmap {
//	public:
//		static constexpr unsigned int NUM_REGS = sizeof...(REGS);
//		WORD_SZ registers[NUM_REGS];
//		bitset<NUM_REGS> seen = {0};
//		std::shared_ptr<Bus> bus;
//		unsigned int devAddr;
//
//		Regmap(std::shared_ptr<Bus> bus, unsigned int addr): bus(std::move(bus)), devAddr(addr) {}
//
//		int read(Register reg, WORD_SZ& dest) {
//			size_t memIdx = addrToIndex(getAddr(reg));
//			bool memoized = memIdx != NUM_REGS;
//			int r;
//
//			if(memoized && bitset_test(seen, memIdx)) {
//				return registers[memIdx];
//			}
//
//			r = bus->read(devAddr, getAddr(reg), &dest, sizeof(WORD_SZ));
//			if(r < 0) {
//				return r;
//			}
//			// we need to fix endianness
//			dest = endianfix::fixEndianess<WORD_SZ, ENDIAN>(dest);
//			if(memoized) {
//				registers[memIdx] = dest;
//				bitset_set(seen, memIdx);
//			}
//			dest = (dest & getBitmask<WORD_SZ>(reg)) >> getLowBit(reg);
//			return 0;
//		}
//		int write(Register reg, WORD_SZ val) {
//			size_t memIdx = addrToIndex(getAddr(reg));
//			bool memoized = memIdx != NUM_REGS;
//			int r;
//			WORD_SZ original;
//			WORD_SZ unendianFixed;
//
//			if(getHighBit(reg) == sizeof(val)-1 && getLowBit(reg) == 0) {
//				// we can skip getting the original value if the register spans the whole thing
//			}
//			else {
//				r = read(reg, original);
//				if(r < 0) {
//					return r;
//				}
//			}
//
//			// we invert the bitmask to wipe out the bits defined by the register, then OR the register's value in
//			unendianFixed = (original & ~getBitmask<WORD_SZ>(reg)) | (val << getLowBit(reg));
//			// fix endianness before we write it back in
//			val = endianfix::fixEndianess<WORD_SZ, ENDIAN>(unendianFixed);
//			r = bus->write(devAddr, getAddr(reg), &val, sizeof(WORD_SZ));
//			if(r < 0) {
//				return r;
//			}
//			if(memoized) {
//				registers[memIdx] = unendianFixed;
//			}
//			return 0;
//		}
//	private:
//		// constexpr OP
//		constexpr size_t addrToIndex(size_t addr) {
//			Register regs[] = {REGS...};
//			for(size_t i = 0; i < NUM_REGS; i++) {
//				if(getAddr(regs[i]) == addr) {
//					return i;
//				}
//			}
//			return NUM_REGS;
//		}
//	};
//}