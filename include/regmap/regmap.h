#pragma once
#include "register.h"
#include "bus.h"
#include "bitstuff.h"
#include <cstdint>
#include "endianfix.h"
#include <tuple>

namespace regmap {
	using endianfix::endian;
	using endianfix::fixEndianess;
	using dev_addr_t = uint16_t;
	/**
	 * An implementation of a register map.
	 *
	 * @tparam WORD_SZ the type of each word in the regmap
	 * @tparam ENDIAN the endianness of the device
	 * @tparam REGS registers to memoize. Make sure that there are no duplicates in this register!
	 */
	template<endian ENDIAN,
		typename... MEMOIZED>
	class Regmap {
	public:
		static constexpr unsigned int NUM_MEMOIZED = sizeof...(MEMOIZED);
		using MemoSzs = std::tuple<RegType<MEMOIZED>...>;
		MemoSzs memoizedRegs;
		bitset<NUM_MEMOIZED> seen = {0};
		dev_addr_t devAddr;
		Bus *bus;

		Regmap(Bus *bus, dev_addr_t addr): bus(bus), devAddr(addr) {}

		// the following are templated on registers
		template<typename REG>
		int read(RegType<REG>& dest) {
			return privRead<RegType<REG>>(RegAddr<REG>(), dest);
		}
		template<typename REG>
		int write(RegType<REG> value) {
			return privWrite<RegType<REG>>(RegAddr<REG>(), value);
		}
		// the following are templated of masks and mask-vals
		template<typename MASK>
		int read(MaskType<MASK>& dest) {
			int r = read<RegOf<MASK>>(dest);
			if(r < 0) {
				return r;
			}
			dest = shiftOutValue<MASK>>(dest);
			return 0;
		}
		template<typename MASK>
		int write(MaskType<MASK> src) {
			// if the new mask spans the whole reg, we don't need the old value
			if(MaskSpansRegister<MASK>()) {
				return write<RegOf<MASK>>(src);
			}
			// otherwise, read in the old value and write out the new one
			MaskType<MASK> newValue;
			int r = read<RegOf<MASK>>(newValue);
			if(r < 0) {
				return r;
			}
			newValue = applyMask<MASK>(newValue, src);
			return write<RegOf<MASK>>(newValue);
		}

		// the following are just utilities
		template<typename REG>
		bool isMemoized() {
			return memoIndex(RegAddr<REG>()) >= 0;
		}
	private:
		// to reduce binary size, we'll template the below only on the size of the register
		template<typename REG_SZ>
		int privRead(reg_addr_t regAddr, REG_SZ &dest) {
			int32_t memoIdx = memoIndex(regAddr);
			if(memoIdx >= 0 && bitset_test(seen, memoIdx)) {
				return *memoPtr<REG_SZ>(memoIdx);
			}
			int r = bus->read(devAddr, regAddr, &dest, sizeof(REG_SZ));
			if(r < 0) {
				return r;
			}
			dest = fixEndianess<ENDIAN>(&dest);
			if(memoIdx >= 0) {
				*memoPtr<REG_SZ>(memoIdx) = dest;
				bitset_set(seen, memoIdx);
			}
			return 0;
		}
		template<typename REG_SZ>
		int privWrite(reg_addr_t regAddr, REG_SZ value) {
			value = fixEndianess<ENDIAN>(&value);
			int r = bus->write(devAddr, regAddr, &value, sizeof(REG_SZ));
			if(r < 0) {
				return r;
			}
			int32_t memoIdx = memoIndex(regAddr);
			if(memoIdx >= 0) {
				*memoPtr<REG_SZ>(memoIdx) = value;
				bitset_set(seen, memoIdx);
			}
			return 0;
		}
		// the below 2 functions should compile to a switch statement
		constexpr int32_t memoIndex(reg_addr_t addr) {
			reg_addr_t regs[] = {MEMOIZED::addr...};
			for(size_t i = 0; i < NUM_MEMOIZED; i++) {
				if(regs[i] == addr) {
					return i;
				}
			}
			return -1;
		}
		template <typename REG_SZ, size_t N = 0>
		REG_SZ* memoPtr(size_t memoIdx) {
			if (N == memoIdx) {
				return &std::get<N>(memoizedRegs);
			}
			if constexpr (N + 1 < std::tuple_size_v<MemoSzs>) {
				return memoPtr<N + 1>(memoIdx);
			}
			else {
				return nullptr;
			}
		}
	};
}