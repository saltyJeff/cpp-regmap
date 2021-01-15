#pragma once
#include <cstdint>

namespace alufix::types {
	template<std::size_t SIZE>
	struct ALUTypeImpl {
		using type = void;
	};
	template<>
	struct ALUTypeImpl<1> {
		using type = uint8_t;
	};
	template<>
	struct ALUTypeImpl<2> {
		using type = uint16_t;
	};
	template<>
	struct ALUTypeImpl<3> {
		using type = uint32_t;
	};
	template<>
	struct ALUTypeImpl<4> {
		using type = uint32_t;
	};
	template<>
	struct ALUTypeImpl<8> {
		using type = uint64_t;
	};
	template<std::size_t SIZE>
	using ALUType = typename ALUTypeImpl<SIZE>::type;

	/**
	 * Comment this out as soon as C++20's new endian stuff is stable:
	 * 	https://en.cppreference.com/w/cpp/types/endian
	 */
	enum class endian {
#ifdef _WIN32
		little = 0,
		big = 1,
		native = little
#else
		little = __ORDER_LITTLE_ENDIAN__,
		big    = __ORDER_BIG_ENDIAN__,
		native = __BYTE_ORDER__
#endif
	};
}