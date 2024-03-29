#pragma once

#include "../detail/ordered_container.hpp"

namespace libra {

	template <
		class Key,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<Key>
	> class ordered_set 
		: public detail::ordered_container
					<
						Key, // container value
						Compare, // key comparator
						Allocator, // container allocator
						detail::identity<Key>, // key extractor
						false // duplicates not allowed
					> 
	{
		using base_type = detail::ordered_container
							<
								Key, // container value
								Compare, // key comparator
								Allocator, // container allocator
								detail::identity<Key>, // key extractor
								false // duplicates not allowed
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

		// ctors
		ordered_set() = default;

		explicit ordered_set(const Compare& comp, const Allocator& alloc = Allocator())
			: base_type(comp, alloc) {}

		explicit ordered_set(const Allocator& alloc)
			: base_type(alloc) {}

		template <class InIt>
		ordered_set(InIt first, InIt last,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: base_type(first, last, comp, alloc) {}

		template <class InIt>
		ordered_set(InIt first, InIt last,
			const Allocator& alloc)
			: base_type(first, last, alloc) {}

		ordered_set(const ordered_set&) = default;
		ordered_set(const ordered_set& other, const Allocator& alloc)
			: base_type(other, alloc) {}

		ordered_set(ordered_set&&) = default;
		ordered_set(ordered_set&& other, const Allocator& alloc)
			: base_type(std::move(other), alloc) {}

		ordered_set(std::initializer_list<value_type> list,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: base_type(list, comp, alloc) {}

		ordered_set(std::initializer_list<value_type> list,
			const Allocator& alloc)
			: base_type(list, alloc) {}

		// dtor
		~ordered_set() = default;

		// assignment
		ordered_set& operator=(const ordered_set&) = default;
		ordered_set& operator=(ordered_set&&) = default;
		ordered_set& operator=(std::initializer_list<value_type> list) {
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

namespace std {
	template <class Key, class Compare, class Allocator>
	void swap(
		libra::ordered_set<Key, Compare, Allocator>& lhs,
		libra::ordered_set<Key, Compare, Allocator>& rhs) noexcept(noexcept(lhs.swap(rhs)))
	{
		lhs.swap(rhs);
	}
}