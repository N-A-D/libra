#pragma once

#include "detail/extract_key.hpp"
#include "detail/ordered_container.hpp"

namespace va {

	namespace dtl = detail;

	template <
		class Key,
		class Value,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<std::pair<Key, Value>>
	> class ordered_multimap
		: public dtl::ordered_container
					<
						Key, // key of value
						std::pair<Key, Value>, // container value
						Compare, // key comparator
						Allocator, // container allocator type
						dtl::select1st<std::pair<Key, Value>>, // key extractor
						true // duplicates allowed
					>
	{
		using base_type = dtl::ordered_container
							<
								Key, // key of value
								std::pair<Key, Value>, // container value
								Compare, // key comparator
								Allocator, // container allocator type
								dtl::select1st<std::pair<Key, Value>>, // key extractor
								true // duplicates allowed
							>;
	public:

		using typename base_type::key_type;
		using mapped_type = Value;
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

		// ctors
		ordered_multimap() = default;

		explicit ordered_multimap(const Compare& comp, const Allocator& alloc = Allocator())
			: base_type(comp, alloc) {}

		explicit ordered_multimap(const Allocator& alloc)
			: base_type(alloc) {}

		template <class InIt>
		ordered_multimap(InIt first, InIt last,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: base_type(first, last, comp, alloc) {}

		template <class InIt>
		ordered_multimap(InIt first, InIt last,
			const Allocator& alloc)
			: base_type(first, last, alloc) {}

		ordered_multimap(const ordered_multimap&) = default;
		ordered_multimap(const ordered_multimap& other, const Allocator& alloc)
			: base_type(other, alloc) {}

		ordered_multimap(ordered_multimap&&) = default;
		ordered_multimap(ordered_multimap&& other, const Allocator& alloc)
			: base_type(std::move(other), alloc) {}

		ordered_multimap(std::initializer_list<value_type> list,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: base_type(list comp, alloc) {}

		ordered_multimap(std::initializer_list<value_type> list,
			const Allocator& alloc)
			: base_type(list, alloc) {}

		// dtor
		~ordered_multimap() = default;

		// assignment
		ordered_multimap& operator=(const ordered_multimap&) = default;
		ordered_multimap& operator=(ordered_multimap&&) = default;
		ordered_multimap& operator=(std::initializer_list<value_type> list) {
			base_type::operator=(list);
			return *this;
		}

		using base_type::get_allocator;

		// iterators
		using base_type::begin;
		using base_type::cbegin;
		using base_type::rbegin;
		using base_type::crbegin;

		using base_type::end;
		using base_type::cend;
		using base_type::rend;
		using base_type::crend;

		// capacity
		using base_type::empty;
		using base_type::size;
		using base_type::max_size;
		using base_type::capacity;
		using base_type::reserve;
		using base_type::shrink_to_fit;

		// modifiers
		using base_type::clear;
		using base_type::insert;
		using base_type::emplace;
		using base_type::emplace_hint;
		using base_type::erase;
		using base_type::swap;

		// lookup
		using base_type::count;
		using base_type::find;
		using base_type::contains;
		using base_type::equal_range;
		using base_type::lower_bound;
		using base_type::upper_bound;

		// observers
		using base_type::key_comp;
		using base_type::value_comp;

	};

}