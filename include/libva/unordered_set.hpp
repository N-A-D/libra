#pragma once

#include "detail/extract_key.hpp"
#include "detail/unordered_container.hpp"

namespace va {

	template <
		class Key,
		class Hash = std::hash<Key>,
		class KeyEqual = std::equal_to<Key>,
		class Allocator = std::allocator<Key>
	> class unordered_set 
		: public detail::unordered_container
					<
						Key, 
						Hash, 
						KeyEqual,
						Allocator, 
						detail::identity<Key>
					>
	{

	};

}