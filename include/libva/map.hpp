#pragma once

#include <initializer_list>
#include "detail/extract_key.hpp"
#include "detail/ordered_container.hpp"

namespace va {

	template <
		class Key,
		class Value,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<std::pair<Key, Value>>
	> class map 
		: detail::ordered_container<Key, Value, Compare, Allocator, detail::select1st<std::pair<Key, Value>>, false> 
	{
		using base_type = detail::ordered_container
							<
								Key, 
								Value, 
								Compare, 
								Allocator, 
								detail::select1st<std::pair<Key, Value>>,
								false
							>;
	public:
		
		using typename base_type::key_type;
		using typename base_type::value_type;
		using typename base_type::size_type;
		using typename base_type::difference_type;
		using typename base_type::key_compare;
		using typename base_type::value_compare;
		using typename base_type::allocator_type;
		using typename base_type::reference;
		using typename base_type::const_reference;
		using typename base_type::pointer;
		using typename base_type::const_pointer;
		using typename base_type::iterator;
		using typename base_type::const_iterator;
		using typename base_type::reverse_iterator; 
		using typename base_type::const_reverse_iterator;

	};

	template <
		class Key,
		class Value,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<std::pair<Key, Value>>
	> class multimap
		: detail::ordered_container<Key, Value, Compare, Allocator, detail::select1st<std::pair<Key, Value>>, true>
	{
		using base_type = detail::ordered_container
							<
								Key,
								Value,
								Compare,
								Allocator,
								detail::select1st<std::pair<Key, Value>>,
								true
							>;
	public:

		using typename base_type::key_type;
		using typename base_type::value_type;
		using typename base_type::size_type;
		using typename base_type::difference_type;
		using typename base_type::key_compare;
		using typename base_type::value_compare;
		using typename base_type::allocator_type;
		using typename base_type::reference;
		using typename base_type::const_reference;
		using typename base_type::pointer;
		using typename base_type::const_pointer;
		using typename base_type::iterator;
		using typename base_type::const_iterator;
		using typename base_type::reverse_iterator;
		using typename base_type::const_reverse_iterator;

	};

}