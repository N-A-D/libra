#pragma once

#include "detail/extract_key.hpp"
#include "detail/unordered_container.hpp"

namespace va {

	template <
		class Key,
		class MappedType,
		class Hash = std::hash<Key>,
		class KeyEqual = std::equal_to<Key>,
		class Allocator = std::allocator<std::pair<Key, MappedType>>
	> class unordered_map
		: public detail::unordered_container
					<
						std::pair<Key, MappedType>,
						Hash,
						KeyEqual,
						Allocator,
						detail::select1st<std::pair<Key, MappedType>>
					>
	{

	};

}