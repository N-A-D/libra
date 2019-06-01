#pragma once 

#include <vector>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <type_traits>

namespace va {
	namespace detail {

		template <
			class Key,
			class Value,
			class Compare,
			class Allocator,
			class ExtractKey,
			bool AllowDuplicates
		> class ordered_container {
			using container_type = std::vector<Value, Allocator>;
		public:

			using key_type               = Key;
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

			key_compare m_key_cmp;
			value_compare m_val_cmp;
			container_type m_data;

			struct EquivalenceRelation {
				Compare m_cmp{};
				bool operator()(const key_type& lhs, const key_type& rhs) const {
					return !m_cmp(lhs, rhs)
						&& !m_cmp(rhs, lhs);
				}
			};

			template <class RndIt>
			RndIt priv_lower_bound(RndIt first, RndIt last, const key_type& key) const {
				ExtractKey key_of;
				std::iterator_traits<RndIt>::difference_type len, step;
				len = last - first;
				RndIt mid;
				while (len > 0) {
					mid = first;
					step = len / 2;
					mid += step;
					if (m_key_cmp(key_of(*mid), key)) {
						first = ++mid;
						len -= step + 1;
					}
					else
						len = step;
				}
				return first;
			}

			template <class RndIt>
			RndIt priv_upper_bound(RndIt first, RndIt last, const key_type& key) const {
				ExtractKey key_of;
				std::iterator_traits<RndIt>::difference_type len, step;
				len = last - first;
				RndIt mid;
				while (len > 0) {
					mid = first;
					step = len / 2;
					mid += step;
					if (!m_key_cmp(key, key_of(*mid))) {
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
				ExtractKey key_of;
				EquivalenceRelation is_equal;

				m_data.emplace_back(std::forward<Args>(args)...);
				auto lower = lower_bound(key_of(m_data.back()));
				if (lower == std::prev(end())) {
					return { lower, true };
				}
				else if (is_equal(key_of(*lower), key_of(m_data.back()))) {
					m_data.pop_back();
					return { lower, false };
				}
				else {
					return { std::rotate(lower, std::prev(end()), end()), true };
				}
			}

			template <class... Args>
			iterator emplace_hint_unique(const_iterator hint, Args&& ...args) {
				assert(iterator_in_range(hint) && "Iterator out of range!");

				ExtractKey key_of;
				EquivalenceRelation is_equal;

				difference_type step = hint - cbegin();
				m_data.emplace_back(std::forward<Args>(args)...);

				iterator pos = begin() + step;
				iterator last = std::prev(end());

				if (pos == last || m_val_cmp(*last, *pos)) {
					if (pos == begin() || m_val_cmp(*std::prev(pos), *last))
						return std::rotate(pos, last, end());
					iterator prev = std::prev(pos);
					if (is_equal(key_of(*prev), key_of(*last))) {
						m_data.pop_back();
						return prev;
					}
					else {
						auto lower = priv_lower_bound(begin(), prev, key_of(*last));
						if (is_equal(key_of(*lower), key_of(*last))) {
							m_data.pop_back();
							return lower;
						}
						else
							return std::rotate(lower, last, end());
					}
				}
				else {
					auto lower = priv_lower_bound(pos, end(), key_of(*last));
					if (lower == last)
						return lower;
					else if (is_equal(key_of(*lower), key_of(*last))) {
						m_data.pop_back();
						return lower;
					}
					else
						return std::rotate(lower, last, end());
				}
			}

			template <class... Args>
			iterator emplace_common(Args&& ...args) {
				ExtractKey key_of;
				m_data.emplace_back(std::forward<Args>(args)...);
				auto upper = upper_bound(key_of(m_data.back()));
				if (upper == end())
					return std::prev(end());
				return std::rotate(upper, std::prev(end()), end());
			}

			template <class... Args>
			iterator emplace_hint_common(const_iterator hint, Args&& ...args) {
				assert(iterator_in_range(hint) && "Iterator out of range!");

				ExtractKey key_of;
				EquivalenceRelation is_equal;

				difference_type step = hint - cbegin();
				m_data.emplace_back(std::forward<Args>(args)...);

				iterator pos = begin() + step;
				iterator last = std::prev(end());

				if (pos == last || m_val_cmp(*last, *pos)) {
					if (pos == begin() || m_val_cmp(*std::prev(pos), *last) || is_equal(key_of(*std::prev(pos)), key_of(*last)))
						return std::rotate(pos, last, end());
					else
						return std::rotate(priv_upper_bound(begin(), std::prev(pos), key_of(*last)), last, end());
				}
				else {
					auto upper = priv_upper_bound(pos, end(), key_of(*last));
					if (upper == end())
						return last;
					return std::rotate(upper, last, end());
				}
			}

			friend bool operator==(
				const ordered_container& lhs,
				const ordered_container& rhs)
			{
				auto comp = lhs.value_comp();
				auto equal = [&comp](const auto& lhs, const auto& rhs) {
					return !comp(lhs, rhs) && !comp(rhs, lhs);
				};
				return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), equal);
			}

			friend bool operator!=(
				const ordered_container& lhs,
				const ordered_container& rhs)
			{
				return !(lhs == rhs);
			}

			friend bool operator<(
				const ordered_container& lhs,
				const ordered_container& rhs)
			{
				return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), lhs.value_comp());
			}


			friend bool operator<=(
				const ordered_container& lhs,
				const ordered_container& rhs)
			{
				return lhs < rhs || lhs == rhs;
			}

			friend bool operator>(
				const ordered_container& lhs,
				const ordered_container& rhs)
			{
				return rhs < lhs;
			}

			friend bool operator>=(
				const ordered_container& lhs,
				const ordered_container& rhs)
			{
				return lhs > rhs || lhs == rhs;
			}

		public:

			// ctor
			ordered_container()
				: ordered_container(Compare(), Allocator()) {}

			explicit ordered_container(const Compare& comp, const Allocator& alloc = Allocator())
				: m_key_cmp(comp)
				, m_val_cmp(comp)
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
				, m_data(other.m_data, alloc) {}

			ordered_container(ordered_container&&) = default;
			ordered_container(ordered_container&& other, const Allocator& alloc)
				: m_key_cmp(std::move(other.m_key_cmp))
				, m_val_cmp(std::move(other.m_val_cmp))
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
			ordered_container& operator=(ordered_container&&) = default;
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

			auto insert(const value_type& value) { return emplace(value); }
			auto insert(value_type&& value) { return emplace(std::move(value)); }

			iterator insert(iterator hint, const value_type& value) { return emplace_hint(hint, value); }
			iterator insert(const_iterator hint, const value_type& value) { return emplace_hint(hint, value); }
			iterator insert(const_iterator hint, value_type&& value) { return emplace_hint(hint, std::move(value)); }

			template <class InIt>
			void insert(InIt first, InIt last) {
				for (auto it = first; it != last; ++it)
					insert(cend(), *it);
			}

			void insert(std::initializer_list<value_type> list) { insert(list.begin(), list.end()); }

			template <class... Args>
			auto emplace(Args&&... args) {
				if constexpr (AllowDuplicates) {
					return emplace_common(std::forward<Args>(args)...);
				}
				else {
					return emplace_unique(std::forward<Args>(args)...);
				}
			}

			template <class... Args>
			auto emplace_hint(const_iterator hint, Args&&... args) {
				if constexpr (AllowDuplicates) {
					return emplace_hint_common(hint, std::forward<Args>(args)...);
				}
				else {
					return emplace_hint_unique(hint, std::forward<Args>(args)...);
				}
			}

			iterator erase(iterator pos) { return m_data.erase(pos); }
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

			void swap(ordered_container& other) {
				m_data.swap(other.m_data);
				std::swap(m_key_cmp, other.m_key_cmp);
				std::swap(m_val_cmp, other.m_val_cmp);
			}

			// lookup
			size_type count(const key_type& key) const {
				auto range = equal_range(key);
				return range.second - range.first;
			}

			iterator find(const key_type& key) {
				EquivalenceRelation is_equal;
				ExtractKey key_of;
				auto lower = lower_bound(key);
				return lower != end() && is_equal(key_of(*lower), key) ? lower : end();
			}

			const_iterator find(const key_type& key) const {
				EquivalenceRelation is_equal;
				ExtractKey key_of;
				auto lower = lower_bound(key);
				return lower != end() && is_equal(key_of(*lower), key) ? lower : end();
			}

			bool contains(const key_type& key) const {
				return find(key) != end();
			}

			std::pair<iterator, iterator> equal_range(const key_type& key) {
				return { lower_bound(key), upper_bound(key) };
			}
			std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
				return { lower_bound(key), upper_bound(key) };
			}

			iterator lower_bound(const key_type& key) {
				return priv_lower_bound(begin(), end(), key); 
			}

			const_iterator lower_bound(const key_type& key) const {
				return priv_lower_bound(begin(), end(), key); 
			}

			iterator upper_bound(const key_type& key) {
				return priv_upper_bound(begin(), end(), key); 
			}

			const_iterator upper_bound(const key_type& key) const {
				return priv_upper_bound(begin(), end(), key);
			}

			// observers
			key_compare key_comp() const { return m_key_cmp; }
			value_compare value_comp() const { return m_val_cmp; }

		};

	}
}