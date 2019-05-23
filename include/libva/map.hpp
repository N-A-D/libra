#pragma once

#include <tuple>
#include <stdexcept>
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
		: public detail::ordered_container<Key, Value, Compare, Allocator, detail::select1st<std::pair<Key, Value>>> 
	{
		using base_type = detail::ordered_container
							<
								Key, 
								Value, 
								Compare, 
								Allocator, 
								detail::select1st<std::pair<Key, Value>>
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
		map() = default;

		explicit map(const Compare& comp, const Allocator& alloc = Allocator())
			: base_type(comp, alloc) {}

		explicit map(const Allocator& alloc)
			: map(Compare(), alloc) {}

		template <class InIt>
		map(InIt first, InIt last,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: map(comp, alloc)
		{
			insert(first, last);
		}

		template <class InIt>
		map(InIt first, InIt last,
			const Allocator& alloc)
			: map(first, last, Compare(), alloc) {}

		map(const map&) = default;
		map(const map& other, const Allocator& alloc)
			: base_type(other, alloc) {}

		map(map&&) = default;
		map(map&& other, const Allocator& alloc)
			: base_type(std::move(other), alloc) {}

		map(std::initializer_list<value_type> list,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: map(list.begin(), list.end(), comp, alloc) {}

		map(std::initializer_list<value_type> list,
			const Allocator& alloc)
			: map(list, Compare(), alloc) {}

		// dtor
		~map() = default;

		// assignment
		map& operator=(const map&) = default;
		map& operator=(map&&) = default;
		map& operator=(std::initializer_list<value_type> list) {
			clear();
			insert(list);
			return *this;
		}

		using base_type::get_allocator;

		// element access
		mapped_type& at(const Key& key) {
			auto it = find(key);
			if (it == end())
				throw std::out_of_range("No such element exists with the given key!");
			else
				return it->second;
		}

		const mapped_type& at(const Key& key) const {
			auto it = find(key);
			if (it == end())
				throw std::out_of_range("No such element exists with the given key!");
			else
				return it->second;
		}

		mapped_type& operator[](const key_type& key) {
			auto it = find(key);
			if (it != end)
				return it->second;
			else
				return this->try_emplace(key).first->second;
		}

		mapped_type& operator[](key_type&& key) {
			auto it = find(key);
			if (it != end)
				return it->second;
			else
				return this->try_emplace(std::move(key)).first->second;
		}

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

		template <class M>
		std::pair<iterator, bool> insert_or_assign(const key_type& k, M&& obj) {
			auto it = find(k);
			if (it != end()) {
				it->second = std::forward<M>(obj);
				return { it, false };
			}
			else
				return emplace(k, std::forward<M>(obj));
		}

		template <class M>
		std::pair<iterator, bool> insert_or_assign(key_type&& k, M&& obj) {
			auto it = find(k);
			if (it != end()) {
				it->second = std::forward<M>(obj);
				return { it, false };
			}
			else
				return emplace(std::move(k), std::forward<M>(obj));
		}

		template <class M>
		std::pair<iterator, bool> insert_or_assign(const_iterator hint, const key_type& k, M&& obj) {
			auto it = find(k);
			if (it != end()) {
				it->second = std::forward<M>(obj);
				return { it, false };
			}
			else
				return emplace_hint(hint, k, std::forward<M>(obj));
		}

		template <class M>
		std::pair<iterator, bool> insert_or_assign(const_iterator hint, key_type&& k, M&& obj) {
			auto it = find(k);
			if (it != end()) {
				it->second = std::forward<M>(obj);
				return { it, false };
			}
			else
				return emplace_hint(hint, std::move(k), std::forward<M>(obj));
		}

		template <class... Args>
		std::pair<iterator, bool> emplace(Args&&... args) {
			return base_type::emplace_unique(std::forward<Args>(args)...);
		}

		template <class... Args>
		iterator emplace_hint(const_iterator hint, Args&&... args) {
			return base_type::emplace_hint_unique(hint, std::forward<Args>(args)...);
		}

		template <class... Args>
		std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args) {
			auto it = find(key);
			if (it != end())
				return { it, false };
			else
				return { emplace(std::piecewise_construct,
						std::forward_as_tuple(key),
						std::forward_as_tuple(std::forward<Args>(args)...)), true };
		}

		template <class... Args>
		std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args) {
			auto it = find(key);
			if (it != end())
				return { it, false };
			else
				return { emplace(std::piecewise_construct,
					std::forward_as_tuple(std::move(key)),
					std::forward_as_tuple(std::forward<Args>(args)...)), true };
		}

		template <class... Args>
		iterator try_emplace(const_iterator hint, const key_type& key, Args&&... args) {
			auto it = find(key);
			if (it != end())
				return it;
			else
				return emplace_hint(hint,
					std::piecewise_construct,
					std::forward_as_tuple(key),
					std::forward_as_tuple(std::forward<Args>(args)...));

		}

		template <class... Args>
		iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args) {
			auto it = find(key);
			if (it != end())
				return it;
			else
				return emplace_hint(hint,
					std::piecewise_construct,
					std::forward_as_tuple(std::move(key)),
					std::forward_as_tuple(std::forward<Args>(args)...));

		}

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
		class Value,
		class Compare = std::less<Key>,
		class Allocator = std::allocator<std::pair<Key, Value>>
	> class multimap
		: public detail::ordered_container<Key, Value, Compare, Allocator, detail::select1st<std::pair<Key, Value>>>
	{
		using base_type = detail::ordered_container
							<
								Key,
								Value,
								Compare,
								Allocator,
								detail::select1st<std::pair<Key, Value>>
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
		multimap() = default;

		explicit multimap(const Compare& comp, const Allocator& alloc = Allocator())
			: base_type(comp, alloc) {}

		explicit multimap(const Allocator& alloc)
			: multimap(Compare(), alloc) {}

		template <class InIt>
		multimap(InIt first, InIt last,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: multimap(comp, alloc)
		{
			insert(first, last);
		}

		template <class InIt>
		multimap(InIt first, InIt last,
			const Allocator& alloc)
			: multimap(first, last, Compare(), alloc) {}

		multimap(const multimap&) = default;
		multimap(const multimap& other, const Allocator& alloc)
			: base_type(other, alloc) {}

		multimap(multimap&&) = default;
		multimap(multimap&& other, const Allocator& alloc)
			: base_type(std::move(other), alloc) {}

		multimap(std::initializer_list<value_type> list,
			const Compare& comp = Compare(),
			const Allocator& alloc = Allocator())
			: multimap(list.begin(), list.end(), comp, alloc) {}

		multimap(std::initializer_list<value_type> list,
			const Allocator& alloc)
			: multimap(list, Compare(), alloc) {}

		// dtor
		~multimap() = default;

		// assignment
		multimap& operator=(const multimap&) = default;
		multimap& operator=(multimap&&) = default;
		multimap& operator=(std::initializer_list<value_type> list) {
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

		iterator insert(const value_type& value) { return emplace(value); }
		iterator insert(value_type&& value) { return emplace(std::move(value)); }

		iterator insert(iterator hint, const value_type& value) { return emplace_hint(hint, value); }
		iterator insert(const_iterator hint, const value_type& value) { return emplace_hint(hint, value); }
		iterator insert(const_iterator hint, value_type&& value) { return emplace_hint(hint, std::move(value)); }

		template <class InIt>
		void insert(InIt first, InIt last) {
			for (auto it = first; it != last; ++it)
				insert(end(), *it);
		}

		void insert(std::initializer_list<value_type> list) { insert(list.begin(), list.end()); }

		template <class... Args>
		iterator emplace(Args&&... args) {
			return base_type::emplace_common(std::forward<Args>(args)...);
		}

		template <class... Args>
		iterator emplace_hint(const_iterator hint, Args&&... args) {
			return base_type::emplace_hint_common(hint, std::forward<Args>(args)...);
		}

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

}