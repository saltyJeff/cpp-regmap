#pragma once
#include "register.h"
#include "bus.h"
#include <cstdint>
#include "endianfix.h"
#include "memoizer.h"

namespace regmap {
	using endianfix::endian;
	using endianfix::fixEndianess;
	using dev_addr_t = uint16_t;
	/**
	 * An implementation of a register map.
	 *
	 * @tparam ENDIAN the endianness of the device
	 * @tparam MEMOIZED registers to memoize. Make sure that there are no duplicates in this register!
	 */
	template<endian ENDIAN,
		typename... MEMOIZED>
	class Regmap {
	public:
		Memoizer<MEMOIZED...> memoized;
		dev_addr_t devAddr;
		Bus *bus;

		Regmap(Bus *bus, dev_addr_t addr): bus(bus), devAddr(addr) {}

		// the following are templated on registers
		template<typename REG>
		int read(RegType<REG>& dest) {
			return privRead<RegType<REG>>(RegAddr<REG>(), dest);
		}
		template<typename REG>
		typename std::enable_if_t<!std::is_same_v<RegType<REG>, void>, int>
	    write(RegType<REG> value) {
			return privWrite<RegType<REG>>(RegAddr<REG>(), value);
		}
		template<typename REG>
		typename std::enable_if_t<std::is_same_v<RegType<REG>, void>, int>
		write() {
			return bus->write(devAddr, RegAddr<REG>(), nullptr, 0);
		}
		// the following are templated of masks
		template<typename HEAD, typename ...REST>
		int read(MaskType<HEAD>& headVal, MaskType<REST>&... restVal) {
			using MergedMask = MergeMasks<HEAD, REST...>;
			MaskType<HEAD> regValue;
			int r = read<RegOf<MergedMask>>(regValue);
			if(r < 0) {
				return r;
			}
			distributeMask<MaskType<HEAD>, HEAD, REST...>(regValue, headVal, restVal...);
			return 0;
		}
		template<typename HEAD, typename ...REST>
		int write(MaskType<HEAD> headVal, MaskType<REST>... restVal) {
			using MergedMask = MergeMasks<HEAD, REST...>;
			auto maskedValue = mergeMasks<HEAD, REST...>(headVal, restVal...);
			// if the new mask spans the whole reg, we don't need the old value
			if(MaskSpansRegister<MergedMask>()) {
				return write<RegOf<MergedMask>>(maskedValue);
			}
			// otherwise, read in the old value and write out the new one
			MaskType<MergedMask> newValue;
			int r = read<RegOf<MergedMask>>(newValue);
			if(r < 0) {
				return r;
			}
			newValue = applyMask<MergedMask>(newValue, maskedValue);
			return write<RegOf<MergedMask>>(newValue);
		}

		// the following are just utilities
		template<typename REG>
		bool isMemoized() {
			return memoized.isMemoized(memoized.getIdx(RegAddr<REG>()));
		}
	private:
		// to reduce binary size, we'll template the below only on the size of the register
		template<typename REG_SZ>
		int privRead(reg_addr_t regAddr, REG_SZ &dest) {
			reg_addr_t memoIdx = memoized.getIdx(regAddr);
			if(memoized.isMemoized(memoIdx) && memoized.isSeen(memoIdx)) {
				return *( (REG_SZ*)memoized.getPtr(regAddr) );
			}
			int r = bus->read(devAddr, regAddr, (uint8_t*)&dest, sizeof(REG_SZ));
			if(r < 0) {
				return r;
			}
			dest = fixEndianess<ENDIAN>(&dest);
			if(memoized.isMemoized(memoIdx)) {
				*( (REG_SZ*)memoized.getPtr(regAddr) ) = dest;
				memoized.setSeen(memoIdx);
			}
			return 0;
		}
		template<typename REG_SZ>
		int privWrite(reg_addr_t regAddr, REG_SZ value) {
			value = fixEndianess<ENDIAN>(&value);
			int r = bus->write(devAddr, regAddr, (uint8_t*)&value, sizeof(REG_SZ));
			if(r < 0) {
				return r;
			}
			reg_addr_t memoIdx = memoized.getIdx(regAddr);
			if(memoized.isMemoized(memoIdx)) {
				*( (REG_SZ*)memoized.getPtr(regAddr) ) = value;
				memoized.setSeen(memoIdx);
			}
			return 0;
		}
	};
}
