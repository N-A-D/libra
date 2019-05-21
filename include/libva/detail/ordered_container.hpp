#pragma once 

#include <vector>
#include <cassert>
#include <iterator>
#include <algorithm>

namespace va {
	namespace detail {

		template <class T>
		struct identity {
			using type = T;
			const T& operator()(const T& t) const {
				return t;
			}
		};

		template <class Pair>
		struct select1st {
			using type = typename Pair::first_type;
			const type& operator()(const Pair& p) const {
				return p.first;
			}
		};

		template <
			class Key,
			class Value,
			class Compare,
			class Allocator,
			class ExtractKey
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

			struct value_compare {
				Compare m_cmp;
				value_compare(Compare c)
					: m_cmp(c) {}
				bool operator()(const Value& lhs, const Value& rhs) const {
					ExtractKey key_of;
					return m_cmp(key_of(lhs), key_of(rhs));
				}
			};

		private:

			struct EquivalenceRelation {
				Compare m_cmp{};
				bool operator()(const Key& lhs, const Key& rhs) const {
					return !m_cmp(lhs, rhs)
						&& !m_cmp(rhs, lhs);
				}
			};

			key_compare m_key_cmp;
			value_compare m_val_cmp;
			container_type m_data;

			template <class RndIt>
			RndIt priv_lower_bound(RndIt first, RndIt last, const Key& key) const {
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
			RndIt priv_upper_bound(RndIt first, RndIt last, const Key& key) const {
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

		protected:

			// ctor
			ordered_container()
				: m_key_cmp(Compare())
				, m_val_cmp(Compare()) 
				, m_data() {}

			explicit ordered_container(const Compare& comp, const Allocator& alloc = Allocator())
				: m_key_cmp(comp)
				, m_val_cmp(comp)
				, m_data(alloc) {}

			explicit ordered_container(const Allocator& alloc)
				: ordered_container(Compare(), alloc) {}

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

			// dtor
			~ordered_container() = default;

			// assignment
			ordered_container& operator=(const ordered_container&) = default;
			ordered_container& operator=(ordered_container&&) = default;

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
				ExtractKey key_of;
				EquivalenceRelation is_equal;

				difference_type step = hint - cbegin();
				m_data.emplace_back(std::forward<Args>(args)...);

				iterator pos = begin() + step;
				iterator last = std::prev(end());

				// New element belongs somewhere in the range [begin(), pos)
				if (pos == last || m_val_cmp(*last, *pos)) {
					// Empty container or new element is the smallest
					if (pos == begin())
						return std::rotate(pos, last, end());
					iterator prev = std::prev(pos);
					// Pos is the correct position for the new element
					if (m_val_cmp(*prev, *last))
						return std::rotate(pos, last, end());
					// The element has an equivalent and must be removed
					else if (is_equal(key_of(*prev), key_of(*last))) {
						m_data.pop_back();
						return prev;
					}
					// Hint wasn't particularly useful...
					// But it limited the range of possible locations to [begin(), prev)
					else {
						auto lower = priv_lower_bound(begin(), prev, key_of(*last));
						if (is_equal(key_of(*lower), key_of(*last))) {
							m_data.pop_back();
							return lower;
						}
						else {
							return std::rotate(lower, last, end());
						}
					}
				}
				// New element belongs somewhere in the range [pos, end())
				else {
					auto lower = priv_lower_bound(pos, end(), key_of(*last));
					if (lower == last) {
						return lower;
					}
					else if (is_equal(key_of(*lower), key_of(*last))) {
						m_data.pop_back();
						return lower;
					}
					else {
						return std::rotate(lower, last, end());
					}
				}
			}

			template <class... Args>
			std::pair<iterator, bool> emplace_common(Args&& ...args) {
				ExtractKey key_of;
				m_data.emplace_back(std::forward<Args>(args)...);
				auto upper = upper_bound(key_of(m_data.back()));
				return { std::rotate(upper, std::prev(end()), end()), true };
			}

			template <class... Args>
			iterator emplace_hint_common(const_iterator hint, Args&& ...args) {
				ExtractKey key_of;
				EquivalenceRelation is_equal;

				difference_type step = hint - cbegin();
				m_data.emplace_back(std::forward<Args>(args)...);

				iterator pos = begin() + step;
				iterator last = std::prev(end());

				if (pos == last || m_val_cmp(*last, *pos)) {
					if (pos == begin())
						return std::rotate(pos, last, end());
					iterator pos = std::prev(pos);
					if (m_val_cmp(*prev, *last) || is_equal(key_of(*prev), key_of(*last)))
						return std::rotate(pos, last, end());
					else {
						auto upper = priv_upper_bound(begin(), prev, key_of(*last));
						return std::rotate(upper, last, end());
					}
				}
				else {
					auto upper = priv_upper_bound(pos, end(), key_of(*last));
					return std::rotate(upper, last, end());
				}
			}

			iterator erase(iterator pos) { return m_data.erase(pos); }
			iterator erase(const_iterator pos) { return m_data.erase(pos); }
			iterator erase(const_iterator first, const_iterator last) { 
				return m_data.erase(first, last);
			}

			size_type erase(const Key& key) {
				auto range = equal_range(key);
				auto count = range.second - range.first;
				erase(range.first, range.second);
				return count;
			}

			// lookup
			size_type count(const Key& key) const {
				auto range = equal_range(key);
				return range.second - range.first;
			}

			iterator find(const Key& key) {
				EquivalenceRelation is_equal;
				ExtractKey key_of;
				auto lower = lower_bound(key);
				return lower != end() && is_equal(key_of(*lower), key) ? lower : end();
			}

			const_iterator find(const Key& key) const {
				EquivalenceRelation is_equal;
				ExtractKey key_of;
				auto lower = lower_bound(key);
				return lower != end() && is_equal(key_of(*lower), key) ? lower : end();
			}

			bool contains(const Key& key) const {
				return find(key) != end();
			}

			std::pair<iterator, iterator> equal_range(const Key& key) {
				return { lower_bound(key), upper_bound(key) };
			}
			std::pair<const_iterator, const_iterator> equal_range(const Key& key) const {
				return { lower_bound(key), upper_bound(key) };
			}

			iterator lower_bound(const Key& key) { 
				return priv_lower_bound(begin(), end(), key); 
			}

			const_iterator lower_bound(const Key& key) const { 
				return priv_lower_bound(begin(), end(), key); 
			}

			iterator upper_bound(const Key& key) { 
				return priv_upper_bound(begin(), end(), key); 
			}

			const_iterator upper_bound(const Key& key) const { 
				return priv_upper_bound(begin(), end(), key);
			}

			// observers
			key_compare key_comp() const { return m_key_cmp; }
			value_compare value_comp() const { return m_val_cmp; }

		};

	}
}