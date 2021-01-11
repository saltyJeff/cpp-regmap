
#pragma once
#include <utility>
#include <tuple>

// varidic max, min, and minmax implementation (named maximum, minimum and min_max to avoid confusion)
// no support for custom comparators (mainly because i'm not sure how they should be specified)

namespace regmap {
	/**
	 * Modified from: https://gist.github.com/thomcc/7739146
	 */
	namespace minmax {
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
	}
	/**
	 * Modified from: https://devblogs.microsoft.com/oldnewthing/20200629-00/?p=103910
	 */
	namespace tuple_idx {
		template<typename T, typename Tuple>
		struct tuple_element_index_helper;
		// base case
		template<typename T>
		struct tuple_element_index_helper<T, std::tuple<>> {static constexpr std::size_t value = 0;};
		// recursive case: success
		template<typename T, typename... Rest>
		struct tuple_element_index_helper<T, std::tuple<T, Rest...>> {
			static constexpr std::size_t value = 0;
			using RestTuple = std::tuple<Rest...>;
			static_assert(
				tuple_element_index_helper<T, RestTuple>::value ==
				std::tuple_size<RestTuple>::value,
				"type appears more than once in tuple");
		};
		// recursive case: fail
		template<typename T, typename First, typename... Rest>
		struct tuple_element_index_helper<T, std::tuple<First, Rest...>> {
			using RestTuple = std::tuple<Rest...>;
			static constexpr std::size_t value =
				1 + tuple_element_index_helper<T, RestTuple>::value;
		};
	}
	/*
	 * Public interfaces
	 */
	template <typename T, typename... Ts>
	using all_same = std::conjunction<std::is_same<T,Ts>...>;

	template <class T, class ...Rest> // requires SameType<T, Rest...>
	inline constexpr T const &
	minimum(T const &first, Rest const &... rest) {
		return minmax::do_min(first, rest...);
	}
	template <class T, class ...Rest> // requires SameType<T, Rest...>
	inline constexpr T const &
	maximum(T const &first, Rest const &... rest) {
		return minmax::do_max(first, rest...);
	}
	template<typename T, typename Tuple>
	inline constexpr int32_t tupleIndex() {
		std::size_t index = tuple_idx::tuple_element_index_helper<T, Tuple>::value;
		return index < std::tuple_size<Tuple>::value ? index : -1;
	}
}