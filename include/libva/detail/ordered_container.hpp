#pragma once 

#include <vector>
#include <cassert>
#include <algorithm>
#include "type_traits.hpp"

namespace va {
	namespace detail {

		template <
			class Value,
			class Compare,
			class Allocator,
			class ExtractKey,
			bool AllowDuplicates
		> class ordered_container {
			using container_type = std::vector<Value, Allocator>;
		public:

			using key_type               = typename ExtractKey::type;
			using value_type             = typename container_type::value_type;
			using size_type              = typename container_type::size_type;
			using difference_type        = typename container_type::difference_type;
			using key_compare            = Compare;
			using allocator_type         = typename container_type::allocator_type;
			using reference              = typename container_type::reference;
			using const_reference        = typename container_type::const_reference;
			using pointer                = typename container_type::pointer;
			using const_pointer          = typename container_type::const_pointer;
			using iterator               = typename container_type::iterator;
			using const_iterator         = typename container_type::const_iterator;
			using reverse_iterator       = typename container_type::reverse_iterator;
			using const_reverse_iterator = typename container_type::const_reverse_iterator;

			class value_compare {
				friend class ordered_container;
			protected:
				Compare m_cmp;
				value_compare(Compare c)
					: m_cmp(c) {}
			public:
				bool operator()(const value_type& lhs, const value_type& rhs) const {
					ExtractKey key_of;
					return m_cmp(key_of(lhs), key_of(rhs));
				}
			};

		private:

			struct CompareEqual {
				Compare m_cmp{};
				CompareEqual(Compare c)
					: m_cmp(c) {}
				bool operator()(const key_type& lhs, const key_type& rhs) const {
					// Two keys compare equal if and only if neither compare less than the other
					return !m_cmp(lhs, rhs)
						&& !m_cmp(rhs, lhs);
				}
			};

			key_compare m_key_cmp;
			value_compare m_val_cmp;
			CompareEqual m_equal;
			ExtractKey m_extract;
			container_type m_data;

			using emplace_return_type = std::conditional_t<AllowDuplicates, iterator, std::pair<iterator, bool>>;

			template <class RndIt, class Key>
			RndIt impl_lower_bound(RndIt first, RndIt last, const Key& key) const {
				std::iterator_traits<RndIt>::difference_type len, step;
				len = last - first;
				RndIt mid;
				while (len > 0) {
					mid = first;
					step = len / 2;
					mid += step;
					if (m_key_cmp(m_extract(*mid), key)) {
						first = ++mid;
						len -= step + 1;
					}
					else
						len = step;
				}
				return first;
			}

			template <class RndIt, class Key>
			RndIt impl_upper_bound(RndIt first, RndIt last, const Key& key) const {
				std::iterator_traits<RndIt>::difference_type len, step;
				len = last - first;
				RndIt mid;
				while (len > 0) {
					mid = first;
					step = len / 2;
					mid += step;
					if (!m_key_cmp(key, m_extract(*mid))) {
						first = ++mid;
						len -= step + 1;
					}
					else
						len = step;
				}
				return first;
			}

			bool iterator_in_range(const_iterator it) const {
				return cbegin() <= it && it <= cend();
			}

			template <class... Args>
			std::pair<iterator, bool> emplace_unique(Args&& ...args) {
				m_data.emplace_back(std::forward<Args>(args)...);
				auto last = std::prev(end());
				auto lower = impl_lower_bound(begin(), last, m_extract(*last));
				if (lower == last)
					return { lower, true };
				else if (m_equal(m_extract(*lower), m_extract(*last))) {
					m_data.pop_back();
					return { lower, false };
				}
				else {
					std::rotate(lower, last, end());
					return { lower, true };
				}
			}

			template <class... Args>
			iterator emplace_hint_unique(const_iterator hint, Args&& ...args) {
				assert(iterator_in_range(hint) && "Iterator out of range!");

				difference_type step = hint - cbegin();
				m_data.emplace_back(std::forward<Args>(args)...);

				iterator pos = begin() + step;
				iterator last = std::prev(end());

				if (pos == last || m_val_cmp(*last, *pos)) {
					if (pos == begin() || m_val_cmp(*std::prev(pos), *last)) {
						std::rotate(pos, last, end());
						return pos;
					}
					iterator prev = std::prev(pos);
					if (m_equal(m_extract(*prev), m_extract(*last))) {
						m_data.pop_back();
						return prev;
					}
					else {
						auto lower = impl_lower_bound(begin(), prev, m_extract(*last));
						if (m_equal(m_extract(*lower), m_extract(*last))) {
							m_data.pop_back();
							return lower;
						}
						else {
							std::rotate(lower, last, end());
							return lower;
						}
					}
				}
				else {
					auto lower = impl_lower_bound(pos, last, m_extract(*last));
					if (lower == last)
						return lower;
					else if (m_equal(m_extract(*lower), m_extract(*last))) {
						m_data.pop_back();
						return lower;
					}
					else {
						std::rotate(lower, last, end());
						return lower;
					}
				}
			}

			template <class... Args>
			iterator emplace_common(Args&& ...args) {
				m_data.emplace_back(std::forward<Args>(args)...);
				auto upper = impl_upper_bound(begin(), std::prev(end()), m_extract(m_data.back()));
				std::rotate(upper, std::prev(end()), end());
				return upper;
			}

			template <class... Args>
			iterator emplace_hint_common(const_iterator hint, Args&& ...args) {
				assert(iterator_in_range(hint) && "Iterator out of range!");

				difference_type step = hint - cbegin();
				m_data.emplace_back(std::forward<Args>(args)...);

				iterator pos = begin() + step;
				iterator last = std::prev(end());

				if (pos == last || m_val_cmp(*last, *pos)) {
					if (pos == begin() 
						|| m_val_cmp(*std::prev(pos), *last) 
						|| m_equal(m_extract(*std::prev(pos)), m_extract(*last))) 
					{
						std::rotate(pos, last, end());
						return pos;
					}
					else {
						auto upper = impl_upper_bound(begin(), std::prev(pos), m_extract(*last));
						std::rotate(upper, last, end());
						return upper;
					}
				}
				else {
					auto upper = impl_upper_bound(pos, last, m_extract(*last));
					std::rotate(upper, last, end());
					return upper;
				}
			}

		public:

			// ctor
			ordered_container()
				: ordered_container(Compare(), Allocator()) {}

			explicit ordered_container(const Compare& comp, const Allocator& alloc = Allocator())
				: m_key_cmp(comp)
				, m_val_cmp(comp)
				, m_equal(comp)
				, m_extract()
				, m_data(alloc) {}

			explicit ordered_container(const Allocator& alloc)
				: ordered_container(Compare(), alloc) {}

			template <class InIt>
			ordered_container(InIt first, InIt last,
				const Compare& comp = Compare(),
				const Allocator& alloc = Allocator())
				: ordered_container(comp, alloc)
			{
				insert(first, last);
			}

			template <class InIt>
			ordered_container(InIt first, InIt last,
				const Allocator& alloc)
				: ordered_container(first, last, Compare(), alloc) {}

			ordered_container(const ordered_container&) = default;
			ordered_container(const ordered_container& other, const Allocator& alloc)
				: m_key_cmp(other.m_key_cmp)
				, m_val_cmp(other.m_val_cmp)
				, m_equal(other.m_equal)
				, m_extract(other.m_extract)
				, m_data(other.m_data, alloc) {}

			ordered_container(ordered_container&&) = default;
			ordered_container(ordered_container&& other, const Allocator& alloc)
				: m_key_cmp(std::move(other.m_key_cmp))
				, m_val_cmp(std::move(other.m_val_cmp))
				, m_equal(std::move(other.m_equal))
				, m_extract(std::move(other.m_extract))
				, m_data(std::move(other.m_data), alloc) {}

			ordered_container(std::initializer_list<value_type> list,
				const Compare& comp = Compare(),
				const Allocator& alloc = Allocator())
				: ordered_container(list.begin(), list.end(), comp, alloc) {}

			ordered_container(std::initializer_list<value_type> list,
				const Allocator& alloc)
				: ordered_container(list, Compare(), alloc) {}

			// dtor
			~ordered_container() = default;

			// assignment
			ordered_container& operator=(const ordered_container&) = default;
			ordered_container& operator=(ordered_container&&) 
				noexcept(
					std::allocator_traits<Allocator>::is_always_equal::value
					&& std::is_nothrow_move_assignable<Compare>::value) = default;
			ordered_container& operator=(std::initializer_list<value_type> list) {
				clear();
				insert(list);
				return *this;
			}

			allocator_type get_allocator() const noexcept { return m_data.get_allocator(); }

			// iterators
			iterator begin() noexcept { return m_data.begin(); }
			const_iterator begin() const noexcept { return m_data.begin(); }
			const_iterator cbegin() const noexcept { return m_data.cbegin(); }

			iterator end() noexcept { return m_data.end(); }
			const_iterator end() const noexcept { return m_data.end(); }
			const_iterator cend() const noexcept { return m_data.cend(); }

			reverse_iterator rbegin() noexcept { return m_data.rbegin(); }
			const_reverse_iterator rbegin() const noexcept { return m_data.rbegin(); }
			const_reverse_iterator crbegin() const noexcept { return m_data.crbegin(); }

			reverse_iterator rend() noexcept { return m_data.rend(); }
			const_reverse_iterator rend() const noexcept { return m_data.rend(); }
			const_reverse_iterator crend() const noexcept { return m_data.crend(); }

			// capacity
			bool empty() const noexcept { return m_data.empty(); }
			size_type size() const noexcept { return m_data.size(); }
			size_type max_size() const noexcept { return m_data.max_size(); }
			size_type capacity() const noexcept { return m_data.capacity(); }
			void reserve(size_type new_cap) { m_data.reserve(new_cap); }
			void shrink_to_fit() { m_data.shrink_to_fit(); }

			// modifiers
			void clear() noexcept { m_data.clear(); }

			emplace_return_type insert(const value_type& value) { return emplace(value); }
			emplace_return_type insert(value_type&& value) { return emplace(std::move(value)); }

			iterator insert(const_iterator hint, const value_type& value) { return emplace_hint(hint, value); }
			iterator insert(const_iterator hint, value_type&& value) { return emplace_hint(hint, std::move(value)); }

			template <class InIt>
			void insert(InIt first, InIt last) {
				for (auto it = first; it != last; ++it)
					insert(cend(), *it);
			}

			void insert(std::initializer_list<value_type> list) { insert(list.begin(), list.end()); }

			template <class... Args>
			emplace_return_type emplace(Args&&... args) {
				if constexpr (AllowDuplicates) {
					return emplace_common(std::forward<Args>(args)...);
				}
				else {
					return emplace_unique(std::forward<Args>(args)...);
				}
			}

			template <class... Args>
			iterator emplace_hint(const_iterator hint, Args&&... args) {
				if constexpr (AllowDuplicates) {
					return emplace_hint_common(hint, std::forward<Args>(args)...);
				}
				else {
					return emplace_hint_unique(hint, std::forward<Args>(args)...);
				}
			}

			iterator erase(const_iterator pos) { return m_data.erase(pos); }
			iterator erase(const_iterator first, const_iterator last) { 
				return m_data.erase(first, last);
			}

			size_type erase(const key_type& key) {
				auto range = equal_range(key);
				auto count = range.second - range.first;
				if (count > 0)
					erase(range.first, range.second);
				return count;
			}

			void swap(ordered_container& other)
				noexcept(std::allocator_traits<Allocator>::is_always_equal::value
					&& std::is_nothrow_swappable<Compare>::value)
			{
				if (this != &other) {
					std::swap(m_key_cmp, other.m_key_cmp);
					std::swap(m_val_cmp, other.m_val_cmp);
					std::swap(m_equal, other.m_equal);
					std::swap(m_extract, other.m_extract);
					std::swap(m_data, other.m_data);
				}
			}

			// lookup
			size_type count(const key_type& key) const {
				auto range = equal_range(key);
				return range.second - range.first;
			}

			template <class Key>
			std::enable_if_t<is_transparent_v<Key, Compare>, size_type>
				count(const Key& key) const {
				auto range = equal_range(key);
				return range.second - range.first;
			}

			iterator find(const key_type& key) {
				auto lower = lower_bound(key);
				return lower != end() && m_equal(m_extract(*lower), key) ? lower : end();
			}

			const_iterator find(const key_type& key) const {
				auto lower = lower_bound(key);
				return lower != end() && m_equal(m_extract(*lower), key) ? lower : end();
			}

			template <class Key>
			std::enable_if_t<is_transparent_v<Key, Compare>, iterator>
				find(const Key& key) {
				auto lower = lower_bound(key);
				return lower != end() && m_equal(m_extract(*lower), key) ? lower : end();
			}

			template <class Key>
			std::enable_if_t<is_transparent_v<Key, Compare>, const_iterator>
				find(const Key& key) const {
				auto lower = lower_bound(key);
				return lower != end() && m_equal(m_extract(*lower), key) ? lower : end();
			}

			bool contains(const key_type& key) const {
				return find(key) != end();
			}

			template <class Key>
			std::enable_if_t<is_transparent_v<Key, Compare>, bool>
				contains(const Key& key) const {
				return find(key) != end();
			}

			std::pair<iterator, iterator> equal_range(const key_type& key) {
				return { lower_bound(key), upper_bound(key) };
			}

			std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
				return { lower_bound(key), upper_bound(key) };
			}

			template <class Key>
			std::enable_if_t<is_transparent_v<Key, Compare>, std::pair<iterator, iterator>>
				equal_range(const Key& key) {
				return { lower_bound(key), upper_bound(key) };
			}
			
			template <class Key>
			std::enable_if_t<is_transparent_v<Key, Compare>, std::pair<const_iterator, const_iterator>>
				equal_range(const Key& key) const {
				return { lower_bound(key), upper_bound(key) };
			}

			iterator lower_bound(const key_type& key) {
				return impl_lower_bound(begin(), end(), key); 
			}

			const_iterator lower_bound(const key_type& key) const {
				return impl_lower_bound(begin(), end(), key); 
			}

			template <class Key>
			std::enable_if_t<is_transparent_v<Key, Compare>, iterator>
				lower_bound(const Key& key) {
				return impl_lower_bound(begin(), end(), key);
			}

			template <class Key>
			std::enable_if_t<is_transparent_v<Key, Compare>, const_iterator>
				lower_bound(const Key& key) const {
				return impl_lower_bound(begin(), end(), key);
			}

			iterator upper_bound(const key_type& key) {
				return impl_upper_bound(begin(), end(), key); 
			}

			const_iterator upper_bound(const key_type& key) const {
				return impl_upper_bound(begin(), end(), key);
			}

			template <class Key>
			std::enable_if_t<is_transparent_v<Key, Compare>, iterator>
				upper_bound(const Key& key) {
				return impl_upper_bound(begin(), end(), key);
			}

			template <class Key>
			std::enable_if_t<is_transparent_v<Key, Compare>, const_iterator>
				upper_bound(const Key& key) const {
				return impl_upper_bound(begin(), end(), key);
			}

			// observers
			key_compare key_comp() const { return m_key_cmp; }
			value_compare value_comp() const { return m_val_cmp; }

		};

		template <class Value, class Compare, class Allocator, class ExtractKey, bool AllowDuplicates>
		bool operator==(
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& lhs,
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& rhs)
		{
			auto comp = lhs.value_comp();
			auto equal = [&comp](const auto& lhs, const auto& rhs) {
				return !comp(lhs, rhs) && !comp(rhs, lhs);
			};
			return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), equal);
		}

		template <class Value, class Compare, class Allocator, class ExtractKey, bool AllowDuplicates>
		bool operator!=(
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& lhs,
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& rhs)
		{
			return !(lhs == rhs);
		}

		template <class Value, class Compare, class Allocator, class ExtractKey, bool AllowDuplicates>
		bool operator<(
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& lhs,
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& rhs)
		{
			return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), lhs.value_comp());
		}

		template <class Value, class Compare, class Allocator, class ExtractKey, bool AllowDuplicates>
		bool operator<=(
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& lhs,
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& rhs)
		{
			return !(rhs < lhs);
		}
		
		
		template <class Value, class Compare, class Allocator, class ExtractKey, bool AllowDuplicates>
		bool operator>(
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& lhs,
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& rhs)
		{
			return rhs < lhs;
		}

		template <class Value, class Compare, class Allocator, class ExtractKey, bool AllowDuplicates>
		bool operator>=(
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& lhs,
			const ordered_container<Value, Compare, Allocator, ExtractKey, AllowDuplicates>& rhs)
		{
			return !(lhs < rhs);
		}

	}
}