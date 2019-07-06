#pragma once

#include <iterator>
#include <type_traits>

namespace libra {
	namespace detail {

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