#pragma once

#include "deque.hpp"

namespace va {

	template <
		class T,
		class Allocator = std::allocator<T>
	> class queue {
	public:

		using container_type  = deque<T, Allocator>;
		using value_type      = typename container_type::value_type;
		using size_type       = typename container_type::size_type;
		using reference       = typename container_type::reference;
		using const_reference = typename container_type::const_reference;

	private:

		container_type m_data;

	public:
		
		// constructors
		queue() : queue(container_type()) {}

		explicit queue(const container_type& cont = container_type())
			: m_data(cont) {}

		explicit queue(container_type&& cont)
			: m_data(std::move(cont)) {}

		queue(const queue& other) = default;
		queue(const queue& other, const Allocator& alloc)
			: m_data(other.m_data, alloc) {}

		queue(queue&& other) = default;
		queue(queue&& other, const Allocator& alloc)
			: m_data(std::move(other.m_data), alloc) {}

		explicit queue(const Allocator& alloc)
			: m_data(alloc) {}

		queue(const container_type& cont, const Allocator& alloc)
			: m_data(cont, alloc) {}
		queue(container_type&& cont, const Allocator& alloc)
			: m_data(std::move(cont), alloc) {}

		// destructor
		~queue() = default;

		// assignment
		queue& operator=(const queue&) = default;
		queue& operator=(queue&&) = default;

		// element access
		reference front() { return const_cast<reference>(const_cast<const queue*>(this)->front()); }
		const_reference front() const { return m_data.front(); }

		reference back() { return const_cast<reference>(const_cast<const queue*>(this)->back()); }
		const_reference back() const { return m_data.back(); }

		// capacity
		bool empty() const noexcept { return m_data.empty(); }
		size_type size() const noexcept { return m_data.size(); }

		// modifiers
		void push(const value_type& value) { m_data.push_back(value); }
		void push(value_type&& value) { m_data.push_back(std::move(value)); }

		template <class... Args>
		void emplace(Args&&... args) { m_data.emplace_back(std::forward<Args>(args)...); }

		void pop() { m_data.pop_front(); }

		void swap(queue& other) noexcept() { 
			if (this != std::addressof(other)) {
				m_data.swap(other.m_data);
			}
		}

	};

}