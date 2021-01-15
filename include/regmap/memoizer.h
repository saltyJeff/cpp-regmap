#pragma once
#include "register.h"
#include "register_utils.h"
#include <cstdint>
#include <type_traits>
#include "bitset.h"

namespace regmap::memoizer {
	// fwd declaration
	template<std::size_t IDX, typename... REGS>
	struct MemoHolder;

	// general case
	template<std::size_t IDX, typename HEAD, typename... REST>
	struct MemoHolder<IDX, HEAD, REST...> {
		RegType<HEAD> value;
		MemoHolder<IDX + 1, REST...> rest;

		constexpr void* getPtr(std::size_t addr) {
			if (addr == RegAddr<HEAD>()) {
				return (void*)&value;
			}
			return rest.getPtr(addr);
		}
		constexpr std::size_t getIdx(std::size_t addr) {
			if (addr == RegAddr<HEAD>()) {
				return IDX;
			}
			return rest.getIdx(addr);
		}
	};

	// the 1-reg case (tail) case.
	template<std::size_t IDX, typename HEAD>
	struct MemoHolder<IDX, HEAD> {
		RegType<HEAD> value;

		constexpr void* getPtr(std::size_t addr) {
			if (addr == RegAddr<HEAD>()) {
				return (void*)&value;
			}
			return nullptr;
		}
		constexpr std::size_t getIdx(std::size_t addr) {
			if (addr == RegAddr<HEAD>()) {
				return IDX;
			}
			return IDX + 1;
		}
	};
	// zero-memoizer. Everything is constexpr return false
	struct ZeroMemoizer {
		constexpr void* getPtr(std::size_t addr) { return nullptr; }
		constexpr std::size_t getIdx(std::size_t addr) { return 0; }

		// the following methods take in indices, not addresses
		constexpr bool isMemoized(std::size_t idx) { return false; }
		constexpr bool isSeen(std::size_t idx) { return false; }
		void setSeen(std::size_t idx) {};
	};
	// N-memoizer. We actually have to implement stuff :(
	template<typename ...REGS>
	struct NMemoizer {
		static constexpr std::size_t NUM_MEMOIZED = sizeof...(REGS);
		MemoHolder<0, REGS...> memos;
		bitset<NUM_MEMOIZED> regSeen = {0};

		constexpr void* getPtr(std::size_t addr) {
			return memos.getPtr(addr);
		}
		constexpr std::size_t getIdx(std::size_t addr) {
			return memos.getIdx(addr);
		}

		// the following methods take in indicies, not addresses
		constexpr bool isMemoized(std::size_t idx) {
			return idx < NUM_MEMOIZED;
		}
		bool isSeen(std::size_t idx) {
			return bitset_test(regSeen, idx);
		}
		void setSeen(std::size_t idx) {
			bitset_set(regSeen, idx);
		}
	};

	// the final memoizer definition
	template<typename...REGS>
	using Memoizer = typename utils::TypeTernary<sizeof...(REGS) == 0, ZeroMemoizer, NMemoizer<REGS...>>::type;
}