#pragma once

#include <initializer_list>
#include "detail/extract_key.hpp"
#include "detail/ordered_container.hpp"

namespace va {

	template <
		class Key,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<Key>
	> class set 
		: detail::ordered_container<Key, Key, Compare, Allocator, detail::identity<Key>, false> 
	{
		using base_type = detail::ordered_container
							<
								Key, 
								Key, 
								Compare, 
								Allocator, 
								detail::identity<Key>,
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

		// ctors
		set() = default;

		explicit set(const Compare& comp, const Allocator& alloc = Allocator())
			: base_type(comp, alloc) {}

		explicit set(const Allocator& alloc)
			: set(Compare(), alloc) {}

		template <class InIt>
		set(InIt first, InIt last,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: set(comp, alloc)
		{
			insert(first, last);
		}

		template <class InIt>
		set(InIt first, InIt last,
			const Allocator& alloc)
			: set(first, last, Compare(), alloc) {}

		set(const set&) = default;
		set(const set& other, const Allocator& alloc)
			: base_type(other, alloc) {}

		set(set&&) = default;
		set(set&& other, const Allocator& alloc)
			: base_type(std::move(other), alloc) {}

		set(std::initializer_list<value_type> list,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: set(list.begin(), list.end(), comp, alloc) {}

		set(std::initializer_list<value_type> list,
			const Allocator& alloc)
			: set(list, Compare(), alloc) {}

		// dtor
		~set() = default;

		// assignment
		set& operator=(const set&) = default;
		set& operator=(set&&) = default;
		set& operator=(std::initializer_list<value_type> list) {
			clear();
			insert(list);
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

		std::pair<iterator, bool> insert(const value_type& value) { return emplace(value); }
		std::pair<iterator, bool> insert(value_type&& value) { return emplace(std::move(value)); }

		iterator insert(iterator hint, const value_type& value) { return emplace_hint(hint, value); }
		iterator insert(const_iterator hint, const value_type& value) { return emplace_hint(hint, value); }
		iterator insert(const_iterator hint, value_type&& value) { return emplace_hint(hint, std::move(value)); }

		template <class InIt>
		void insert(InIt first, InIt last) {
			for (auto it = first; it != last; ++it)
				insert(end(), *it);
		}

		void insert(std::initializer_list<value_type> list) { insert(list.begin(), list.end()); }

		using base_type::emplace;
		using base_type::emplace_hint;

		using base_type::erase;

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

	template <
		class Key,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<Key>
	> class multiset
		: detail::ordered_container<Key, Key, Compare, Allocator, detail::identity<Key>, true>
	{
		using base_type = detail::ordered_container
							<
								Key,
								Key,
								Compare,
								Allocator,
								detail::identity<Key>,
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