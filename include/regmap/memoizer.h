#pragma once
#include "register.h"
#include <cstdint>
#include <type_traits>
#include "bitset.h"

namespace regmap {
	// fwd declaration
	template<reg_addr_t IDX, typename... REGS>
	struct MemoHolder;

	// general case
	template<reg_addr_t IDX, typename HEAD, typename... REST>
	struct MemoHolder<IDX, HEAD, REST...> {
		RegType<HEAD> value;
		MemoHolder<IDX + 1, REST...> rest;

		constexpr void* getPtr(reg_addr_t addr) {
			if (addr == RegAddr<HEAD>()) {
				return (void*)&value;
			}
			return rest.getPtr(addr);
		}
		constexpr reg_addr_t getIdx(reg_addr_t addr) {
			if (addr == RegAddr<HEAD>()) {
				return IDX;
			}
			return rest.getIdx(addr);
		}
	};

	// the 1-reg case (tail) case.
	template<reg_addr_t IDX, typename HEAD>
	struct MemoHolder<IDX, HEAD> {
		RegType<HEAD> value;

		constexpr void* getPtr(reg_addr_t addr) {
			if (addr == RegAddr<HEAD>()) {
				return (void*)&value;
			}
			return nullptr;
		}
		constexpr reg_addr_t getIdx(reg_addr_t addr) {
			if (addr == RegAddr<HEAD>()) {
				return IDX;
			}
			return IDX + 1;
		}
	};
	// zero-memoizer. Everything is constexpr return false
	struct ZeroMemoizer {
		constexpr void* getPtr(reg_addr_t addr) { return nullptr; }
		constexpr reg_addr_t getIdx(reg_addr_t addr) { return 0; }

		// the following methods take in indices, not addresses
		constexpr bool isMemoized(reg_addr_t idx) { return false; }
		constexpr bool isSeen(reg_addr_t idx) { return false; }
		void setSeen(reg_addr_t idx) {};
	};
	// N-memoizer. We actually have to implement stuff :(
	template<typename ...REGS>
	struct NMemoizer {
		static constexpr reg_addr_t NUM_MEMOIZED = sizeof...(REGS);
		MemoHolder<0, REGS...> memos;
		bitset<NUM_MEMOIZED> regSeen = {0};

		constexpr void* getPtr(reg_addr_t addr) {
			return memos.getPtr(addr);
		}
		constexpr reg_addr_t getIdx(reg_addr_t addr) {
			return memos.getIdx(addr);
		}

		// the following methods take in indicies, not addresses
		constexpr bool isMemoized(reg_addr_t idx) {
			return idx < NUM_MEMOIZED;
		}
		bool isSeen(reg_addr_t idx) {
			return bitset_test(regSeen, idx);
		}
		void setSeen(reg_addr_t idx) {
			bitset_set(regSeen, idx);
		}
	};

	// utility class to pick which memoizer is applicable
	template<bool B, typename ...REGS>
	struct MemoizerPicker{};

	template<>
	struct MemoizerPicker<true> {
		using type = ZeroMemoizer;
	};

	template<typename ...REGS>
	struct MemoizerPicker<false, REGS...> {
		using type = NMemoizer<REGS...>;
	};

	// the final memoizer definition
	template<typename...REGS>
	using Memoizer = typename MemoizerPicker<sizeof...(REGS) == 0, REGS...>::type;
}