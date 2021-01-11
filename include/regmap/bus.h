#pragma once
#include <cstdint>
namespace regmap {
	/**
	 * Implements a bus which can send and receive bytes
	 */
	class Bus {
	public:
		virtual int read(unsigned int deviceAddr, uint16_t regAddr, uint8_t *dest, unsigned int num) = 0;
		virtual int write(unsigned int deviceAddr, uint16_t regAddr, uint8_t *src, unsigned int num) = 0;
		virtual ~Bus() = default;
	};
}