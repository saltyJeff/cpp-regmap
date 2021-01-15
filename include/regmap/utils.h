/*
 * I was wary of #include <algorithm>, since we only need the const
 * versions of min and max, and want to throw on actually invoking
 * them at runtime
 */
#pragma once

namespace regmap::utils {
	/**
	 * Modified from: https://gist.github.com/thomcc/7739146
	 */
	// max base case
	template <class T>
	constexpr T const &do_max(T const &v) {
		return v;
	}

	// max inductive case
	template <class T, class... Rest> // requires SameType<T, Rest...>
	constexpr T const &do_max(T const &v0, T const &v1, Rest const &... rest) {
		return do_max(v0 < v1 ? v1 : v0, rest...);
	}

	// min base case
	template <class T>
	constexpr T const &do_min(T const &v) {
		return v;
	}

	// min variadic inductive case
	template <class T, class... Rest> // requires SameType<T, Rest...>
	constexpr T const &do_min(T const &v0, T const &v1, Rest const &...rest) {
		return do_min(v0 < v1 ? v0 : v1, rest...);
	}

	/*
	 * Public interfaces
	 */

	template <class T, class ...Rest> // requires SameType<T, Rest...>
	inline constexpr T const &
	minimum(T const &first, Rest const &... rest) {
		return do_min(first, rest...);
	}
	template <class T, class ...Rest> // requires SameType<T, Rest...>
	inline constexpr T const &
	maximum(T const &first, Rest const &... rest) {
		return do_max(first, rest...);
	}

	// ternary type: select type b ? T1 : T2
	template<bool B, typename T1, typename T2>
	struct TypeTernary{};

	template<typename T1, typename T2>
	struct TypeTernary<true, T1, T2> {
		using type = T1;
	};

	template<typename T1, typename T2>
	struct TypeTernary<false, T1, T2> {
		using type = T2;
	};

	template<typename HEAD, typename ...REST>
	struct GetHeadImpl {
		using type = HEAD;
	};
	template<typename ...ITEMS>
	using GetHead = typename GetHeadImpl<ITEMS...>::type;

	template <typename ...Ts>
	using all_same_types = std::conjunction<std::is_same<GetHead<Ts>,Ts>...>;

}
namespace regmap {
	using DeviceAddr = unsigned int;
}