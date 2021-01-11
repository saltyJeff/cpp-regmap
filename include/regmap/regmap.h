#pragma once
#include "register.h"
#include "bus.h"
#include "bitstuff.h"
#include <memory>
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
		std::shared_ptr<Bus> bus;

		Regmap(std::shared_ptr<Bus> bus, dev_addr_t addr): bus(std::move(bus)), devAddr(addr) {}

		// the following are templated on registers
		template<typename REG>
		int read(RegType<REG>& dest) {
			return privRead<RegType<REG>>(RegAddr<REG>(), dest);
		}
		template<typename REG>
		int write(RegType<REG> value) {
			return privWrite<RegType<REG>>(RegAddr<REG>(), value);
		}
		// the following are templated of mask-vals

		// the following are just utilities
		template<typename REG>
		bool isMemoized() {
			return memoIndex(RegAddr<REG>()) >= 0;
		}
	private:
		// to reduce binary size, we'll template the below only on the size of the register
		template<typename REG_SZ>
		int privRead(reg_addr_t regAddr, REG_SZ& dest) {
			const int32_t memoIdx = memoIndex(regAddr);
			if(memoIdx >= 0 && bitset_test(seen, memoIdx)) {
				return std::get<memoIdx>(memoizedRegs);
			}
			int r = bus->read(devAddr, regAddr, &dest, sizeof(REG_SZ));
			if(r < 0) {
				return r;
			}
			dest = fixEndianess<REG_SZ>(dest);
			if(memoIdx >= 0) {
				std::get<memoIdx>(memoizedRegs) = dest;
				bitset_set(seen, memoIdx);
			}
			return 0;
		}
		template<typename REG_SZ>
		int privWrite(reg_addr_t regAddr, REG_SZ value) {
			value = fixEndianess<REG_SZ>(value);
			int r = bus->write(devAddr, regAddr, &value, sizeof(REG_SZ));
			if(r < 0) {
				return r;
			}
			const int32_t memoIdx = memoIndex(regAddr);
			if(memoIdx >= 0) {
				std::get<memoIdx>(memoizedRegs) = value;
				bitset_set(seen, memoIdx);
			}
			return 0;
		}
		// this function should compile to a switch statement
		constexpr int32_t memoIndex(reg_addr_t addr) {
			reg_addr_t regs[] = {MEMOIZED::addr...};
			for(size_t i = 0; i < NUM_MEMOIZED; i++) {
				if(regs[i] == addr) {
					return i;
				}
			}
			return -1;
		}
	};
}