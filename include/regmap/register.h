#pragma once
#include <cstdint>
#include <type_traits>
#include "bitstuff.h"
#include "endianfix.h"
#include "utils.h"

namespace regmap {
	/* Register definitions */
	using reg_addr_t = uint16_t;
	using endianfix::uint24_t;

	template<reg_addr_t reg_addr, typename reg_size>
	struct Register {
		static_assert(
			std::is_integral<reg_size>::value,
			// || std::is_same<reg_size, uint24_t>::value, deal with dumb 24bit stuff later
			"Registers must be backed by integer values");
		static constexpr reg_addr_t addr = reg_addr;
		using backingType = reg_size;
	};
	template<typename REG>
	constexpr reg_addr_t RegAddr() {
		return REG::addr;
	}
	template<typename REG>
	using RegType = typename REG::backingType;

	/* Mask definitions */
	template<typename REG, uint8_t mask_high, uint8_t mask_low>
	struct RegMask {
		using backingReg = REG;
		static constexpr uint8_t maskHigh = mask_high;
		static constexpr uint8_t maskLow = mask_low;
	};
	template <typename MASK>
	using RegOf = typename MASK::backingReg;
	template <typename MASK>
	constexpr uint8_t MaskH() {
		return MASK::maskHigh;
	}
	template<typename MASK>
	constexpr uint8_t MaskL() {
		return MASK::maskLow;
	}
	template<typename MASK>
	using MaskType = RegType<RegOf<MASK>>;
	template <typename MASK>
	constexpr bool MaskSpansRegister() {
		size_t registerWidth = sizeof(MaskType<MASK>);
		return MaskH<MASK>() == (registerWidth * 8 - 1) && MaskL<MASK>() == 0;
	}
	/* Bitmask utility */
	template<typename MASK>
	constexpr MaskType<MASK> bitmask() {
		using WORD_SZ = MaskType<MASK>;
		WORD_SZ i = WORD_SZ(); // a safe way to initialize the variable to 0
		i = ~i; // now flip all the bits before we mask
		return ~( (WORD_SZ)i << MASK::maskHigh << 1) & (i << MASK::maskLow);
	}
	/* Register mask utility to shift in and out mask values */
	template<typename MASK>
	constexpr MaskType<MASK> shiftInValue(MaskType<MASK> value) {
		return (value << MASK::maskLow) & bitmask<MASK>();
	}
	template<typename MASK>
	constexpr MaskType<MASK> applyMask(MaskType<MASK> original, MaskType<MASK> newVal) {
		return (original & ~bitmask<MASK>()) | shiftInValue<MASK>(newVal);
	}
	template<typename MASK>
	constexpr MaskType<MASK> shiftOutValue(MaskType<MASK> value) {
		return (value & bitmask<MASK>()) >> MASK::maskLow;
	}
	/* Register mask merging */
	// the value version
	template <typename... REST>
	constexpr auto mergeMasks(MaskType<REST> ... values) {
		return (... | shiftInValue<REST>(values));
	}

	// the type version (so easy)
	template <typename HEAD, typename... REST>
	using MergeMasks = std::enable_if_t<
		all_same<RegOf<HEAD>, RegOf<REST>...>::value,
		RegMask<
			RegOf<HEAD>,
			maximum(HEAD::maskHigh, REST::maskHigh...),
			minimum(HEAD::maskLow, REST::maskLow...)
		>
	>;
	/* Mask-value pair definition */
	template<typename MASK, MaskType<MASK> VALUE>
	struct MaskVal {
		using mask = MASK;
		static constexpr MaskType<MASK> val = VALUE;
	};
	template<typename MASK_VALUE>
	using MaskOf = typename MASK_VALUE::mask;

	/* final merge definition */
	template <typename HEAD, typename... TAIL>
	using MergeMaskValues = MaskVal<
	    MergeMasks<MaskOf<HEAD>, MaskOf<TAIL>...>,
        mergeMasks<MaskOf<HEAD>, MaskOf<TAIL>...>(HEAD::val, TAIL::val...)
    >;
}

#define REG(NAME, ADDR, HIGHBIT, LOWBIT) constexpr regmap::Register NAME = regmap::newRegister(ADDR, HIGHBIT, LOWBIT);
#define REG_BYTE(NAME, ADDR) constexpr regmap::Register NAME = regmap::newRegister(ADDR, 7, 0);