#pragma once

#include <memory>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include "detail/type_traits.hpp"

namespace va {
	namespace detail {
		template <
			class Deque,
			bool IsConst = false
		> class deque_iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type        = typename Deque::value_type;
			using difference_type   = typename Deque::difference_type;
			using reference = std::conditional_t
				<
					IsConst,
					typename Deque::const_reference,
					typename Deque::reference
				>;
			using pointer = std::conditional_t
				<
					IsConst,
					typename Deque::const_pointer,
					typename Deque::pointer
				>;

			friend class deque_iterator<Deque, !IsConst>;

		private:

			bool valid() const noexcept {
				return m_data != nullptr;
			}

			template <bool Constness>
			bool compatible(const deque_iterator<Deque, Constness>& it) const noexcept {
				return m_data == it.m_data;
			}

			pointer address() const noexcept {
				return m_data->linear_address(m_it);
			}

			const Deque* m_data = nullptr;
			pointer m_it = nullptr;

		public:

			deque_iterator() = default;

			deque_iterator(const Deque* data, pointer it)
				: m_data(data), m_it(it) {}

			// non const to const iterator
			deque_iterator(const deque_iterator<Deque, false>& itr)
				: m_data(itr.m_data)
				, m_it(itr.m_it) {}

			deque_iterator& operator=(const deque_iterator& it) {
				if (this != std::addressof(it)) {
					m_data = it.m_data;
					m_it = it.m_it;
				}
				return *this;
			}

			// difference operator

			template <bool Constness>
			difference_type operator-(const deque_iterator<Deque, Constness>& it) const {
				return address() - it.address();
			}

			// pointer-like operators

			reference operator*() const {
				return *(operator->());
			}

			pointer operator->() const {
				assert(valid() && "Invalid calling iterator!");
				return m_it;
			}

			reference operator[](difference_type n) const {
				return *(*this + n);
			}

			// increment

			deque_iterator& operator++() {
				assert(valid());
				assert(m_it && "Increment out of bounds!");
				m_it = m_data->next(m_it);
				if (m_it == m_data->m_tail)
					m_it = nullptr;
				return *this;
			}

			deque_iterator operator++(int) {
				deque_iterator tmp(*this);
				++*this;
				return tmp;
			}

			// decrement

			deque_iterator& operator--() {
				assert(valid());
				assert(m_it != m_data->m_head && "Decrement out of bounds!");
				if (m_it == nullptr)
					m_it = m_data->m_tail;
				m_it = m_data->prev(m_it);
				return *this;
			}

			deque_iterator operator--(int) {
				deque_iterator tmp(*this);
				--*this;
				return tmp;
			}

			// addition

			deque_iterator& operator+=(difference_type n) {
				assert(valid());
				if (n == 0)
					return *this;
				else if (n > 0) {
					assert(m_data->end() - *this >= n && "Increment out of bounds!");
					m_it = m_data->next(m_it, n);
					if (m_it == m_data->m_tail)
						m_it = nullptr;
					return *this;
				}
				else {
					return (*this -= -n);
				}
			}

			deque_iterator operator+(difference_type n) const {
				return deque_iterator(*this) += n;
			}

			// subtraction

			deque_iterator& operator-=(difference_type n) {
				assert(valid());
				if (n == 0)
					return *this;
				else if (n > 0) {
					assert(*this - m_data->begin() >= n && "Decrement out of bounds!");
					if (m_it == nullptr)
						m_it = m_data->m_tail;
					m_it = m_data->prev(m_it, n);
					return *this;
				}
				else {
					return (*this += -n);
				}
			}

			deque_iterator operator-(difference_type n) const {
				return deque_iterator(*this) -= n;
			}

			// equality

			template <bool Constness>
			bool operator==(const deque_iterator<Deque, Constness>& it) const {
				assert(valid() && "Invalid calling iterator!");
				assert(compatible(it) && "Incompatible iterators!");
				return m_it == it.m_it;
			}

			template <bool Constness>
			bool operator!=(const deque_iterator<Deque, Constness>& it) const {
				return !(*this == it);
			}

			template <bool Constness>
			bool operator<(const deque_iterator<Deque, Constness>& it) const {
				assert(valid() && "Invalid calling iterator!");
				assert(compatible(it) && "Incompatible iterator!");
				return address() < it.address();
			}

			template <bool Constness>
			bool operator<=(const deque_iterator<Deque, Constness>& it) const {
				return !(it < *this);
			}

			template <bool Constness>
			bool operator>(const deque_iterator<Deque, Constness>& it) const {
				return it < *this;
			}

			template <bool Constness>
			bool operator>=(const deque_iterator<Deque, Constness>& it) const {
				return !(*this < it);
			}

		};
	}

	template <
		class T,
		class Allocator = std::allocator<T>
	> class deque {
		using alloc_traits = std::allocator_traits<Allocator>;
	public:

		using value_type             = T;
		using allocator_type         = Allocator;
		using size_type              = std::size_t;
		using difference_type        = std::ptrdiff_t;
		using reference              = value_type&;
		using const_reference        = const value_type&;
		using pointer                = typename alloc_traits::pointer;
		using const_pointer          = typename alloc_traits::const_pointer;
		using iterator               = detail::deque_iterator<deque>;
		using const_iterator         = detail::deque_iterator<deque, true>;
		using reverse_iterator       = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		friend class iterator;
		friend class const_iterator;

		// constructors
		deque() : deque(Allocator()) {}

		explicit deque(const Allocator& alloc)
			: m_alloc(alloc)
		{
			initialize();
		}

		explicit deque(size_type n, const Allocator& alloc = Allocator())
			: deque(alloc)
		{
			initialize(n);
		}

		deque(size_type n, const value_type& v, const Allocator& alloc = Allocator())
			: deque(alloc)
		{
			initialize(n, v);
		}

		template <class InIt, class = std::enable_if_t<detail::is_iterator_v<InIt>>>
		deque(InIt first, InIt last, const Allocator& alloc = Allocator())
			: deque(alloc)
		{
			initialize(first, last);
		}

		deque(const deque& other)
			: deque(alloc_traits::select_on_container_copy_construction(other.get_allocator()))
		{
			initialize(other.begin(), other.end());
		}

		deque(const deque& other, const Allocator& alloc)
			: deque(alloc)
		{
			initialize(other.begin(), other.end());
		}

		deque(deque&& other)
			: m_data(other.m_data)
			, m_limit(other.m_limit)
			, m_head(other.m_head)
			, m_tail(other.m_tail)
			, m_size(other.m_size)
			, m_alloc(std::move(other.m_alloc))
		{
			other.initialize();
		}

		deque(deque&& other, const Allocator& alloc)
			: deque(alloc) 
		{
			if constexpr (alloc_traits::is_always_equal::value) {
				m_data = other.m_data;
				m_limit = other.m_limit;
				m_head = other.m_head;
				m_tail = other.m_tail;
				m_size = other.m_size;
				other.initialize();
			}
			else {
				initialize(std::move_iterator(other.begin()), std::move_iterator(other.end()));
				other.initialize();
			}
		}

		deque(std::initializer_list<value_type> list, const Allocator& alloc = Allocator())
			: deque(list.begin(), list.end(), alloc) {}

		// destructor
		~deque() { uninitialize(); }

		// assignment

		deque& operator=(const deque& other);
		deque& operator=(deque&& other) noexcept(alloc_traits::is_always_equal::value);

		deque& operator=(std::initializer_list<value_type> list) {
			assign(list);
			return *this;
		}

		void assign(size_type n, const value_type& value) {
			uninitialize();
			initialize(n, value);
		}

		template <class InIt, class = std::enable_if_t<detail::is_iterator_v<InIt>>>
		void assign(InIt first, InIt last) {
			uninitialize();
			initialize(first, last);
		}

		void assign(std::initializer_list<value_type> list) {
			assign(list.begin(), list.end());
		}

		allocator_type get_allocator() const noexcept { return m_alloc; }

		// element access

		reference at(size_type index) {
			return const_cast<reference>(const_cast<const deque*>(this)->at(index));
		}

		const_reference at(size_type index) const {
			assert(!empty());
			if (!valid_index(index))
				throw std::out_of_range("Index out of range!");
			return (*this)[index];
		}

		reference operator[](size_type index) {
			return const_cast<reference>((*const_cast<const deque*>(this))[index]);
		}

		const_reference operator[](size_type index) const {
			assert(!empty());
			assert(valid_index(index));
			return *next(m_head, index);
		}

		reference front() { 
			return const_cast<reference>(const_cast<const deque*>(this)->front());
		}

		const_reference front() const { 
			assert(!empty()); 
			return *m_head; 
		}

		reference back() { 
			return const_cast<reference>(const_cast<const deque*>(this)->back());
		}

		const_reference back() const {
			assert(!empty());
			return *prev(m_tail);
		}

		// iterators

		iterator begin() noexcept { return iterator(this, m_head); }
		const_iterator begin() const noexcept { return const_iterator(this, m_head); }
		const_iterator cbegin() const noexcept { return const_iterator(this, m_head); }

		iterator end() noexcept { return iterator(this, nullptr); }
		const_iterator end() const noexcept { return const_iterator(this, nullptr); }
		const_iterator cend() const noexcept { return const_iterator(this, nullptr); }

		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(cend()); }
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

		reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(cbegin()); }
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

		// capacity
		bool empty() const noexcept { return size() == 0; }
		bool full() const noexcept { return capacity() == size(); }
		size_type size() const noexcept { return m_size; }
		size_type max_size() const noexcept { return alloc_traits::max_size(m_alloc); }
		size_type capacity() const noexcept { return m_limit - m_data; }
		void shrink_to_fit() { release_unused_memory(); }

		// modifiers

		void clear() noexcept { uninitialize(); }

		iterator insert(const_iterator pos, const value_type& value) { return emplace(pos, value); }
		iterator insert(const_iterator pos, value_type&& value) { return emplace(pos, std::move(value)); }
		iterator insert(const_iterator pos, size_type n, const value_type& value) {
			if (n == 0)
				return end();
			iterator ret = emplace(pos, value);
			--n;
			while (n != 0) {
				ret = emplace(ret, value);
				--n;
			}
			return ret;
		}

		template <class InIt>
		iterator insert(const_iterator pos, InIt first, InIt last) {
			if (first == last)
				return end();
			InIt it = last;
			iterator ret = emplace(pos, --*it);
			while (it != first)
				ret = emplace(ret, --*it);
			return ret;
		}

		iterator insert(const_iterator pos, std::initializer_list<value_type> list) {
			return insert(pos, list.begin(), list.end());
		}

		template <class... Args>
		iterator emplace(const_iterator pos, Args&&... args) 
		{ return emplace_at(pos, std::forward<Args>(args)...); }

		iterator erase(const_iterator pos) { return remove_at(pos); }
		iterator erase(const_iterator first, const_iterator last) {
			assert(last != cend() && first != cend());
			if (first == last)
				return end();
			const_iterator it = last;
			iterator ret = erase(--it);
			while (it != first)
				ret = erase(--it);
			return ret;
		}

		void push_front(const T& value) { append_front(value); }
		void push_front(T&& value) { append_front(std::move(value)); }

		void push_back(const T& value) { append_back(value); }
		void push_back(T&& value) { append_back(std::move(value)); }

		void pop_front() { remove_front(); }
		void pop_back() { remove_back(); }

		template <class... Args>
		void emplace_front(Args&&... args) {
			append_front(std::forward<Args>(args)...);
		}

		template <class... Args>
		void emplace_back(Args&&... args) {
			append_back(std::forward<Args>(args)...);
		}

		void resize(size_type n) {
			if (m_size < n) {
				while (m_size != n)
					append_back();
			}
			else if (m_size > n) {
				while (m_size != n)
					pop_back();
			}
		}

		void resize(size_type n, const value_type& value) {
			if (m_size < n) {
				while (m_size != n)
					append_back(value);
			}
			else if (m_size > n) {
				while (m_size != n)
					pop_back();
			}
		}

		void swap(deque& other) noexcept(alloc_traits::is_always_equal::value) {
			if (this != std::addressof(other)) {
				std::swap(m_data, other.m_data);
				std::swap(m_limit, other.m_limit);
				std::swap(m_head, other.m_head);
				std::swap(m_tail, other.m_tail);
				std::swap(m_size, other.m_size);
				std::swap(m_alloc, other.m_alloc);
			}
		}

	private:

		// Member variables

		pointer m_data = nullptr;  // physical start of the container
		pointer m_limit = nullptr; // physical end of the container (one past)
		pointer m_head = nullptr;  // logical start of the container
		pointer m_tail = nullptr;  // logical end of the container (one past)
		size_type m_size = 0;      // the number of physically occupied slots
		allocator_type m_alloc;

	private:

		// memory management
		
		template <class... Args>
		void construct_element(pointer pos, Args&&... args) {
			alloc_traits::construct(m_alloc, pos, std::forward<Args>(args)...);
		}

		void destroy_element(pointer p) {
			alloc_traits::destroy(m_alloc, p);
		}

		void initialize() {
			m_size = 0;
			m_data = m_limit = m_head = m_tail = nullptr;
		}

		void initialize(size_type n) {
			m_size = n;
			m_data = alloc_traits::allocate(m_alloc, m_size);
			m_limit = m_data + m_size;
			std::uninitialized_default_construct(m_data, m_limit);
			m_head = m_tail = m_data;
		}

		void initialize(size_type n, const value_type& value) {
			m_size = n;
			m_data = alloc_traits::allocate(m_alloc, m_size);
			m_limit = m_data + m_size;
			std::uninitialized_fill(m_data, m_limit, value);
			m_head = m_tail = m_data;
		}

		template <class InIt, class = std::enable_if_t<detail::is_iterator_v<InIt>>>
		void initialize(InIt first, InIt last) {
			m_size = std::distance(first, last);
			m_data = alloc_traits::allocate(m_alloc, m_size);
			m_limit = std::uninitialized_copy(first, last, m_data);
			m_head = m_tail = m_data;
		}

		void uninitialize() {
			if (m_data != nullptr) {
				while (!empty())
					remove_back();
				alloc_traits::deallocate(m_alloc, m_data, capacity());
				initialize(); // Reinitialize to empty container
			}
		}

		void grow() {
			size_type new_cap = std::max(2 * capacity(), size_type(1));
			pointer new_data = alloc_traits::allocate(m_alloc, new_cap);
			if (m_tail > m_head) {
				if constexpr (std::is_move_constructible_v<T>)
					std::uninitialized_move(m_head, m_tail, new_data);
				else
					std::uninitialized_copy(m_head, m_tail, new_data);
			}
			else {
				if constexpr (std::is_move_constructible_v<T>) {
					pointer tmp = std::uninitialized_move(m_head, m_limit, new_data);
					std::uninitialized_move(m_data, m_tail, tmp);
				}
				else {
					pointer tmp = std::uninitialized_copy(m_head, m_limit, new_data);
					std::uninitialized_copy(m_data, m_tail, tmp);
				}
			}
			size_type new_size = m_size;
			uninitialize();
			m_size = new_size;
			m_data = m_head = new_data;
			m_tail = m_data + m_size;
			m_limit = m_data + new_cap;
		}

		void release_unused_memory() {
			if (!full()) {
				pointer new_data = alloc_traits::allocate(m_alloc, m_size);
				if (m_tail > m_head) {
					if constexpr (std::is_move_constructible_v<T>)
						std::uninitialized_move(m_head, m_tail, new_data);
					else
						std::uninitialized_copy(m_head, m_tail, new_data);
				}
				else {
					if constexpr (std::is_move_constructible_v<T>) {
						pointer tmp = std::uninitialized_move(m_head, m_limit, new_data);
						std::uninitialized_move(m_data, m_tail, tmp);
					}
					else {
						pointer tmp = std::uninitialized_copy(m_head, m_limit, new_data);
						std::uninitialized_copy(m_data, m_tail, tmp);
					}
				}
				size_type new_size = m_size;
				uninitialize();
				m_size = new_size;
				m_data = m_head = new_data;
				m_limit = m_tail = m_data + new_size;
			}
		}

		// helpers

		bool valid_index(size_type index) const noexcept {
			return index < m_size;
		}

		template <class Pointer>
		Pointer next(Pointer p, difference_type n = 1) const noexcept {
			return p + ((m_limit - p) > n ? n : n - (m_limit - m_data));
		}

		template <class Pointer>
		Pointer prev(Pointer p, difference_type n = 1) const noexcept {
			return p - ((p - m_data) >= n ? n : n - (m_limit - m_data));
		}

		template <class Pointer>
		Pointer linear_address(Pointer p) const noexcept {
			if (p == nullptr)
				return m_limit;
			else {
				if (p < m_head)
					return p + (m_limit - m_head);
				else
					return m_data + (p - m_head);
			}
		}

		// element insertion

		template <class... Args>
		iterator emplace_at(const_iterator pos, Args&&... args) {
			if (pos == cend()) {
				append_back(std::forward<Args>(args)...);
				return std::prev(end());
			}
			if (pos == cbegin()) {
				append_front(std::forward<Args>(args)...);
				return begin();
			}
			difference_type off_from_front = pos - cbegin();
			difference_type off_from_back = cend() - pos;
			if (off_from_front < off_from_back) {
				append_front(std::forward<Args>(args)...);
				reverse_iterator it = rbegin() + off_from_back;
				it = std::rotate(it, std::prev(rend()), rend());
				return it.base();
			}
			else {
				append_back(std::forward<Args>(args)...);
				iterator it = begin() + off_from_front;
				it = std::rotate(it, std::prev(end()), end());
				return it;
			}
		}

		iterator remove_at(const_iterator pos) {
			assert(pos != cend());
			if (std::next(pos) == end())
				remove_back();
			if (pos == cbegin())
				remove_front();
			difference_type off_from_front = pos - cbegin();
			difference_type off_from_back = cend() - pos;
			if (off_from_front < off_from_back) { 
				reverse_iterator it = rbegin() + off_from_back;
				it = std::rotate(it, it + 1, rend());
				remove_front();
				return it.base();
			}
			else {
				iterator it = begin() + off_from_front;
				it = std::rotate(it, it + 1, end());
				remove_back();
				return it;
			}
		}

		template <class... Args>
		void append_front(Args&&... args) {
			if (empty() || full())
				grow();
			m_head = prev(m_head);
			construct_element(m_head, std::forward<Args>(args)...);
			++m_size;
		}

		void remove_front() {
			assert(!empty());
			destroy_element(m_head);
			m_head = next(m_head);
			--m_size;
		}

		template <class... Args>
		void append_back(Args&&... args) {
			if (empty() || full())
				grow();
			construct_element(m_tail, std::forward<Args>(args)...);
			m_tail = next(m_tail);
			++m_size;
		}

		void remove_back() {
			assert(!empty());
			m_tail = prev(m_tail);
			destroy_element(m_tail);
			--m_size;
		}

	};

	template <class T, class Allocator>
	bool operator==(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class T, class Allocator>
	bool operator!=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return !(lhs == rhs);
	}

	template <class T, class Allocator>
	bool operator<(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class T, class Allocator>
	bool operator<=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return !(rhs < lhs);
	}

	template <class T, class Allocator>
	bool operator>(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return rhs < lhs;
	}

	template <class T, class Allocator>
	bool operator>=(const deque<T, Allocator>& lhs, const deque<T, Allocator>& rhs) {
		return !(lhs < rhs);
	}

}

namespace std {

	template <class T, class Allocator>
	void swap(va::deque<T, Allocator>& lhs, va::deque<T, Allocator>& rhs)
		noexcept(noexcept(lhs.swap(rhs)))
	{
		lhs.swap(rhs);
	}

}