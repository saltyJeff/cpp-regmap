#pragma once
#include <cstdint>
#include <type_traits>
#include "bitset.h"
#include "alufix.h"
#include "utils.h"

namespace regmap {
	/**
	 * Definition for 24-bit integers
	 */
	using uint24_t = uint8_t[3];

	/**
	 * Defines a register on a device
	 * @tparam ADDR_TYPE the width of the address of the register
	 * @tparam ADDR the address of the register
	 * @tparam REG_WIDTH the width of the register
	 * @tparam REG_TYPE the type of the register. This may not be the same
	 * as an integer of REG_WIDTH in case of 24-bit values
	 */
	template<std::size_t ADDR_WIDTH, std::size_t ADDR, std::size_t REG_WIDTH>
	struct Register {
		static constexpr std::size_t AddrWidth = ADDR_WIDTH;
		static constexpr std::size_t RegWidth = REG_WIDTH;
		static constexpr std::size_t addr = ADDR;
	};
	/**
	 * Convenience alias to Register
	 */
	template<typename ADDR_TYPE, ADDR_TYPE ADDR, typename REG_TYPE>
	using Reg = Register<sizeof(ADDR_TYPE), ADDR, sizeof(REG_TYPE)>;

	template<typename ADDR_TYPE, ADDR_TYPE ADDR>
	using Cmd = Register<sizeof(ADDR_TYPE), ADDR, 0>;

	/**
	 * Defines a mask of an existing register
	 * @tparam REG The register being masked
	 * @tparam mask_high The high bit of the mask
	 * @tparam mask_low The low bit of the mask
	 */
	template<typename REG, uint8_t MASK_HIGH, uint8_t MASK_LOW>
	struct RegMask {
		using Reg = REG;
		static constexpr uint8_t MaskHigh = MASK_HIGH;
		static constexpr uint8_t MaskLow = MASK_LOW;
	};
}
/* The following macros will require a type ADDR_TYPE to be predefined */
#define DECLR_REG( NAME, ADDR, SZ ) using NAME = regmap::Reg<ADDR_TYPE, ADDR, SZ>;
#define DECLR_MASK( NAME, REG, HIGH, LOW ) using NAME = regmap::RegMask<REG, HIGH, LOW>;
#define DECLR_CMD( NAME, ADDR ) using NAME = regmap::Cmd<ADDR_TYPE, ADDR>;