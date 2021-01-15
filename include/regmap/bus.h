#pragma once
#include <cstdint>
#include "utils.h"
namespace regmap {

	/**
	 * Implements a bus which can send and receive bytes
	 */
	class Bus {
	public:
		virtual int read(DeviceAddr deviceAddr, uint8_t *regPtr, uint8_t regWidth, uint8_t *dest, uint8_t num) = 0;
		virtual int write(DeviceAddr deviceAddr, uint8_t *regPtr, uint8_t regWidth, uint8_t *src, uint8_t num) = 0;
		virtual ~Bus() = default;
	};
}