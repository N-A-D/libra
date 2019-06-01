#pragma once

#include <initializer_list>
#include "detail/extract_key.hpp"
#include "detail/unordered_container.hpp"

namespace va {

	template <
		class Key,
		class Hash = std::hash<Key>,
		class EqualKey = std::equal_to<Key>,
		class Allocator
	> class unordered_set 
		: public detail::unordered_container<Key, Key, Hash, EqualKey, Allocator, detail::identity<Key>>
	{
		using base_type = detail::unordered_container<Key, Key, Hash, EqualKey, Allocator, detail::identity<Key>>
	public:
	};

}