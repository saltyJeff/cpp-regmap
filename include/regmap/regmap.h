#pragma once
#include "register.h"
#include "register_utils.h"
#include "bus.h"
#include <cstdint>
#include "alufix.h"
#include "memoizer.h"

namespace regmap {
	using alufix::endian;
	using alufix::toLocalALUFormat;
	using alufix::toDeviceFormat;
	using alufix::fixEndianness;
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
		memoizer::Memoizer<MEMOIZED...> memoized;
		DeviceAddr devAddr;
		Bus *bus;

		Regmap(Bus *bus, DeviceAddr addr): bus(bus), devAddr(addr) {}

		/**
		 * Read a register
		 * @tparam REG The register to read
		 * @param dest The destination to store the value
		 * @return negative on error
		 */
		template<typename REG>
		int read(RegType<REG>& dest) {
			RegAddrType<REG> addr = RegAddr<REG>();
			auto memoIdx = memoized.getIdx(addr);
			fixEndianness<ENDIAN>(addr);
			return directRead(memoIdx, &addr, sizeof(addr), &dest, RegWidth<REG>());
		}
		/**
		 * Write a register
		 * @tparam REG The register to write
		 * @param value The value to write into the regmap
		 * @return negative on error
		 */
		template<typename REG>
		int write(RegType<REG> value) {
			RegAddrType<REG> addr = RegAddr<REG>();
			auto memoIdx = memoized.getIdx(addr);
			fixEndianness<ENDIAN>(addr);
			return directWrite(memoIdx, &addr, sizeof(addr), &value, RegWidth<REG>());
		}
		/**
		 * Writes a command (specialization of register)
		 * @tparam REG The command to write
		 * @return
		 */
		template<typename REG>
		std::enable_if_t<REG::RegWidth == 0, int> write() {
			RegAddrType<REG> addr = RegAddr<REG>();
			fixEndianness<ENDIAN>(addr);
			return directWrite(memoized.NUM_MEMOIZED, &addr, sizeof(addr), nullptr, 0);
		}
		/**
		 * Read a register and distribute its value across multiple masks
		 * @tparam HEAD The first mask to read into
		 * @tparam REST The other masks
		 * @param headVal The reference for the first mask
		 * @param restVal The reference for the other masks
		 * @return negative on error
		 */
		template<typename ...MASKS>
		int read(MaskType<MASKS>&... values) {
			using MergedMask = MergeMasks<MASKS...>;
			using RegType = MaskType<utils::GetHead<MASKS...>>;
			RegType regValue;
			int r = read<RegOf<MergedMask>>(regValue);
			if(r < 0) {
				return r;
			}
			distributeMask<RegType, MASKS...>(regValue, values...);
			return 0;
		}
		/**
		 * Write a register using its component masks
		 * @tparam HEAD The first mask to write out
		 * @tparam REST The other masks to write out
		 * @param headVal The first mask value to write
		 * @param restVal The other mask values to write
		 * @return
		 */
		template<typename ...MASKS>
		int write(MaskType<MASKS>... values) {
			using MergedMask = MergeMasks<MASKS...>;
			auto maskedValue = mergeMasks<MASKS...>(values...);
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

		/**
		 * Returns whether a register is memoized
		 * @tparam REG the register to check
		 * @return true if it is memoized
		 */
		template<typename REG>
		bool isMemoized() {
			return memoized.isMemoized(memoized.getIdx(RegAddr<REG>()));
		}
		template<typename REG>
		std::size_t memoIdx(RegAddrType<REG> addr) {
			return memoized.getIdx(addr);
		}
		/*
		 * The following are direct implementations of regmap reading & writing.
		 * *in the case where num is odd, make sure the void*'s can access num+1 bytes*
		 * *when writing, your incoming pointer will be scrambled*
		 * Beware: No type safety for you
		 */
		int directRead(std::size_t memoIdx, void* regAddr, uint8_t regSz, void* dest, uint8_t num) {
			if(memoized.isMemoized(memoIdx) && memoized.isSeen(memoIdx)) {
				alufix::memcpy(dest, memoized.getPtr(memoIdx), num);
				return 0;
			}
			auto *destPtr = reinterpret_cast<uint8_t*>(dest);
			auto r = bus->read(devAddr, reinterpret_cast<uint8_t*>(regAddr), regSz, destPtr, num);
			if(r < 0) {
				return r;
			}
			alufix::toLocalALUFormat<ENDIAN>(destPtr, destPtr, num);
			if(memoized.isMemoized(memoIdx)) {
				alufix::memcpy(memoized.getPtr(memoIdx), destPtr, num);
				memoized.setSeen(memoIdx);
			}
			return 0;
		}
		int directWrite(std::size_t memoIdx, void* regAddr, uint8_t regSz, void* src, uint8_t num) {
			uint8_t toSend[num];
			alufix::toDeviceFormat<ENDIAN>(src, toSend, num);
			int r = bus->write(devAddr, reinterpret_cast<uint8_t*>(regAddr), regSz, toSend, num);
			if(r < 0) {
				return r;
			}
			if(memoized.isMemoized(memoIdx)) {
				alufix::memcpy(memoized.getPtr(memoIdx), src, num);
				memoized.setSeen(memoIdx);
			}
			return 0;
		}
	};
}
