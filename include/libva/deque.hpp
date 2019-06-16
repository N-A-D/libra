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
			using value_type = typename Deque::value_type;
			using difference_type = typename Deque::difference_type;
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

		private:

			const Deque* m_data = nullptr;
			pointer m_it = nullptr;
			
		public:

			deque_iterator() = default;

			deque_iterator(const Deque* data, pointer it)
				: m_data(data), m_it(it) {}

			// nonconst to const iterator
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

			difference_type operator-(const deque_iterator& it) const;

			// pointer-like operators

			reference operator*() const;
			pointer operator->() const;
			reference operator[](difference_type n) const;

			// increment

			deque_iterator& operator++();
			deque_iterator& operator++(int);

			// decrement

			deque_iterator& operator--();
			deque_iterator& operator--(int);

			// addition

			deque_iterator& operator+=(difference_type n);
			deque_iterator& operator+(difference_type n) const;

			// subtraction

			deque_iterator& operator-=(difference_type n);
			deque_iterator& operator-(difference_type n) const;

			// equality

			bool operator==(const deque_iterator& it) const;
			bool operator!=(const deque_iterator& it) const;

			bool operator<(const deque_iterator& it) const;
			bool operator<=(const deque_iterator& it) const;

			bool operator>(const deque_iterator& it) const;
			bool operator>=(const deque_iterator& it) const;

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
		using iterator               = detail::deque_iterator<deque<T, Allocator>>;
		using const_iterator         = detail::deque_iterator<deque<T, Allocator>, true>;
		using reverse_iterator       = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		friend class iterator;
		friend class const_iterator;

		// constructor
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

		deque(deque&& other);
		deque(deque&& other, const Allocator& alloc);

		deque(std::initializer_list<value_type> list, const Allocator& alloc = Allocator())
			: deque(list.begin(), list.end(), alloc) {}

		// destructor
		~deque() { uninitialize(); }

		// assignment

		deque& operator=(const deque& other);
		deque& operator=(deque&& other) noexcept(alloc_traits::is_always_equal::value);
		deque& operator=(std::initializer_list<value_type> list);

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
			assert(!empty());
			if (!valid_index(index))
				throw std::out_of_range("Index out of range!");
			pointer p = m_head + index;
			if (p >= m_limit)
				p = m_data + (p - m_limit);
			return *p;
		}

		const_reference at(size_type index) const {
			assert(!empty());
			if (!valid_index(index))
				throw std::out_of_range("Index out of range!");
			pointer p = m_head + index;
			if (p >= m_limit)
				p = m_data + (p - m_limit);
			return *p;
		}

		reference operator[](size_type index) {
			assert(!empty());
			assert(valid_index(index));
			pointer p = m_head + index;
			if (p >= m_limit)
				p = m_data + (p - m_limit);
			return *p;
		}

		const_reference operator[](size_type index) const {
			assert(!empty());
			assert(valid_index(index));
			pointer p = m_head + index;
			if (p >= m_limit)
				p = m_data + (p - m_limit);
			return *p;
		}

		reference front() { 
			assert(!empty()); 
			return *m_head; 
		}

		const_reference front() const { 
			assert(!empty()); 
			return *m_head; 
		}

		reference back() { 
			assert(!empty());
			return *std::prev((m_tail == m_data ? m_limit : m_tail));
		}

		const_reference back() const {
			assert(!empty());
			return *std::prev((m_tail == m_data ? m_limit : m_tail));
		}

		// iterators

		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		const_iterator cbegin() const noexcept;

		iterator end() noexcept;
		const_iterator end() const noexcept;
		const_iterator cend() const noexcept;

		reverse_iterator rbegin() noexcept;
		const_reverse_iterator rbegin() const noexcept;
		const_reverse_iterator crbegin() const noexcept;

		reverse_iterator rend() noexcept;
		const_reverse_iterator rend() const noexcept;
		const_reverse_iterator crend() const noexcept;

		// capacity
		bool empty() const noexcept { return size() == 0; }
		bool full() const noexcept { return capacity() == size(); }
		size_type size() const noexcept { return m_size; }
		size_type max_size() const noexcept { return alloc_traits::max_size(m_alloc); }
		size_type capacity() const noexcept { return m_limit - m_data; }
		
		// modifiers
		void clear() noexcept { uninitialize(); }

		iterator insert(const_iterator pos, const value_type& value);
		iterator insert(const_iterator pos, value_type&& value);
		iterator insert(iterator pos, size_type n, const value_type& value);

		template <class InIt>
		iterator insert(iterator pos, InIt first, InIt last);

		iterator insert(const_iterator pos, std::initializer_list<value_type> list);

		template <class... Args>
		iterator emplace(const_iterator pos, Args&&... args);

		iterator erase(const_iterator pos);
		iterator erase(const_iterator first, iterator last);

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

	private:

		// Member variables

		pointer m_data = nullptr;  // physical start of the container
		pointer m_limit = nullptr; // physical end of the container (one past)
		pointer m_head = nullptr;  // logical start of the container
		pointer m_tail = nullptr;  // logical end of the container (one past)
		size_type m_size = 0;      // the number of physically occupied slots
		allocator_type m_alloc;

	private:

		// helpers

		bool valid_index(size_type index) const noexcept {
			return index < m_size;
		}

		// Memory management
		
		void initialize() {
			m_size = 0;
			m_data = m_limit = m_head = m_tail = nullptr;
		}

		void initialize(size_type n) {
			m_size = n;
			m_data = alloc_traits::allocate(m_alloc, m_size);
			m_limit = m_data + m_size;
			std::uninitialized_default_construct(m_data, m_limit);
			m_head = m_data;
			m_tail = m_limit;
		}

		void initialize(size_type n, const value_type& value) {
			m_size = n;
			m_data = alloc_traits::allocate(m_alloc, m_size);
			m_limit = m_data + m_size;
			std::uninitialized_fill(m_data, m_limit, value);
			m_head = m_data;
			m_tail = m_limit;
		}

		template <class InIt, class = std::enable_if_t<detail::is_iterator_v<InIt>>>
		void initialize(InIt first, InIt last) {
			m_size = std::distance(first, last);
			m_data = alloc_traits::allocate(m_alloc, m_size);
			m_limit = std::uninitialized_copy(first, last, m_data);
			m_head = m_data;
			m_tail = m_limit;
		}

		void uninitialize() {
			if (m_data != nullptr) {
				if (!empty()) {
					pointer it = m_tail;
					if (m_tail > m_head) {
						while (it != m_head)
							alloc_traits::destroy(m_alloc, --it);
					}
					else {
						while (it != m_data)
							alloc_traits::destroy(m_alloc, --it);
						it = m_limit;
						while (it != m_head)
							alloc_traits::destroy(m_alloc, --it);
					}
				}
				alloc_traits::deallocate(m_alloc, m_data, capacity());
				initialize();
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
			m_data = new_data;
			m_limit = m_data + new_cap;
			m_size = new_size;
			m_head = m_data;
			m_tail = m_data + m_size;
		}

		// Element construction

		template <class... Args>
		void append_front(Args&&... args) {
			if (empty() || full())
				grow();
			if (m_head == m_data)
				m_head = m_limit;
			alloc_traits::construct(m_alloc, --m_head, std::forward<Args>(args)...);
			++m_size;
		}

		template <class... Args>
		void append_back(Args&&... args) {
			if (empty() || full())
				grow();
			if (m_tail == m_limit)
				m_tail = m_data;
			alloc_traits::construct(m_alloc, m_tail++, std::forward<Args>(args)...);
			++m_size;
		}

		void remove_front() {
			assert(!empty());
			--m_size;
			alloc_traits::destroy(m_alloc, m_head++);
			if (m_head == m_limit)
				m_head = m_data;
		}

		void remove_back() {
			assert(!empty());
			--m_size;
			alloc_traits::destroy(m_alloc, --m_tail);
			if (m_tail == m_data)
				m_tail == m_limit;
		}

	};

}
