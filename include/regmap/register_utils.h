#pragma once
#include "register.h"
#include "alufix_types.h"
namespace regmap {
	/** Define member accessors for Register **/
	template <typename REG>
	std::size_t RegAddr() {
		return REG::addr;
	}
	template <typename REG>
	std::size_t RegWidth() {
		return REG::regWidth;
	}
	template <typename REG>
	using RegAddrType = typename REG::AddrType;
	template<typename REG>
	using RegType = alufix::types::ALUTypeImpl<RegAddr<REG>()>;


	/** Define member accessors for RegMask **/
	template <typename MASK>
	using RegOf = typename MASK::Reg;
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

	/** Register mask utilities **/
	template <typename MASK>
	constexpr bool MaskSpansRegister() {
		auto registerWidth = sizeof(MaskType<MASK>);
		return MaskH<MASK>() == (registerWidth * 8 - 1) && MaskL<MASK>() == 0;
	}
	template<typename MASK>
	constexpr MaskType<MASK> bitmask() {
		using WORD_SZ = MaskType<MASK>;
		WORD_SZ i = WORD_SZ(); // a safe way to initialize the variable to 0
		i = ~i; // now flip all the bits before we mask
		return ~( (WORD_SZ)i << MASK::maskHigh << 1) & (i << MASK::maskLow);
	}

	/** shifting in values to masks **/
	template<typename MASK>
	constexpr MaskType<MASK> shiftInValue(MaskType<MASK> value) {
		return (value << MASK::maskLow) & bitmask<MASK>();
	}
	template<typename MASK>
	constexpr MaskType<MASK> shiftOutValue(MaskType<MASK> value) {
		return (value & bitmask<MASK>()) >> MASK::maskLow;
	}
	template<typename MASK>
	constexpr MaskType<MASK> applyMask(MaskType<MASK> original, MaskType<MASK> newVal) {
		return (original & ~bitmask<MASK>()) | shiftInValue<MASK>(newVal);
	}
	template<typename T, typename ...MASKS>
	inline void distributeMask(T value, MaskType<MASKS>&... masks) {
		(... | (masks = shiftOutValue<MASKS>(value)) );
	}

	/** Register mask merging **/
	template <typename... REST>
	constexpr auto mergeMasks(MaskType<REST> ... values) {
		return (... | shiftInValue<REST>(values));
	}
	template <typename HEAD, typename... REST>
	using MergeMasks = std::enable_if_t<
		utils::all_same_types<RegOf<HEAD>, RegOf<REST>...>::value,
		RegMask<
			RegOf<HEAD>,
			maximum(HEAD::maskHigh, REST::maskHigh...),
			minimum(HEAD::maskLow, REST::maskLow...)
		>
	>;
}