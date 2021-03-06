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
	 * @tparam ADDR the address of the register
	 * @tparam REG_WIDTH the width of the register
	 * @tparam REG_TYPE the type of the register. This may not be the same
	 * as an integer of REG_ADDR_WIDTH in case of 24-bit values
	 */
	template<std::size_t ADDR, std::size_t REG_WIDTH>
	struct Register {
		static constexpr std::size_t RegWidth = REG_WIDTH;
		static constexpr std::size_t addr = ADDR;
	};
	/**
	 * Convenience alias to Register
	 */
	template<std::size_t ADDR, typename REG_TYPE>
	using Reg = std::enable_if_t<!std::is_void_v<REG_TYPE>, Register<ADDR, sizeof(REG_TYPE)>>;

	template<std::size_t ADDR>
	using Cmd = Register<ADDR, 0>;

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
#define DECLR_REG( NAME, ADDR, SZ ) using NAME = regmap::Reg<ADDR, SZ>;
#define DECLR_MASK( NAME, REG, HIGH, LOW ) using NAME = regmap::RegMask<REG, HIGH, LOW>;
#define DECLR_CMD( NAME, ADDR ) using NAME = regmap::Cmd<ADDR>;
#define DECLR_BYTE( NAME, ADDR ) using NAME = regmap::Reg<ADDR, uint8_t>;