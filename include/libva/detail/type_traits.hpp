#pragma once

#include <iterator>
#include <type_traits>

namespace va {
	namespace detail {

		template <
			class It,
			class = void
		> struct is_iterator : std::false_type {};

		template <
			class It
		> struct is_iterator<It, std::void_t<typename std::iterator_traits<It>::iterator_category>>
			: std::true_type {};

		template <
			class It
		> constexpr bool is_iterator_v = is_iterator<It>::value;

		template <
			class Key,
			class Compare,
			class = void
		> struct is_transparent
			: std::false_type {};

		template <
			class Key,
			class Compare
		> struct is_transparent<Key, Compare, std::void_t<typename Compare::is_transparent>>
			: std::true_type {};

		template <
			class Key,
			class Compare
		> constexpr bool is_transparent_v = is_transparent<Key, Compare>::value;

	}
}