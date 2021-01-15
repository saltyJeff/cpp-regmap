#pragma once
#include "register.h"
#include "alufix_types.h"
namespace regmap {
	/** Define member accessors for Register **/
	template <typename REG>
	constexpr std::size_t RegAddr() {
		return REG::addr;
	}
	template <typename REG>
	constexpr std::size_t RegWidth() {
		return REG::RegWidth;
	}
	template<typename REG>
	using RegType = alufix::types::ALUType<REG::RegWidth>; // cannot have intermediate constexpr calls


	/** Define member accessors for RegMask **/
	template <typename MASK>
	using RegOf = typename MASK::Reg;
	template <typename MASK>
	constexpr uint8_t MaskH() {
		return MASK::MaskHigh;
	}
	template<typename MASK>
	constexpr uint8_t MaskL() {
		return MASK::MaskLow;
	}
	template<typename MASK>
	using MaskType = alufix::types::ALUType<MASK::Reg::RegWidth>; // cannot have intermediate constexpr calls

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
		return ~( (WORD_SZ)i << MASK::MaskHigh << 1) & (i << MASK::MaskLow);
	}

	/** shifting in values to masks **/
	template<typename MASK>
	constexpr MaskType<MASK> shiftInValue(MaskType<MASK> value) {
		return (value << MASK::MaskLow) & bitmask<MASK>();
	}
	template<typename MASK>
	constexpr MaskType<MASK> shiftOutValue(MaskType<MASK> value) {
		return (value & bitmask<MASK>()) >> MASK::MaskLow;
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
	template <typename... MASKS>
	constexpr auto mergeMasks(MaskType<MASKS> ... values) {
		return (... | shiftInValue<MASKS>(values)) >> utils::minimum(MASKS::MaskLow...);
	}
	template <typename ...MASKS>
	struct MergeMasksImpl {
		static_assert(utils::all_same_types<RegOf<MASKS>...>::value,
			"Only masks of the same register can be masked");
		using type = RegMask<
			RegOf<utils::GetHead<MASKS...>>,
			utils::maximum(MASKS::MaskHigh...),
			utils::minimum(MASKS::MaskLow...)
		>;
	};
	template<typename ...MASKS>
	using MergeMasks = typename MergeMasksImpl<MASKS...>::type;
}