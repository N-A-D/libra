#include <gtest/gtest.h>
#include <deque>
#include <vector>
#include <random>
#include <algorithm>
#include "detail/constants.hpp"
#include "../include/libva/deque.hpp"

template <class T>
struct my_allocator { // For testing copy/move constructors and assignment operators
	
	using value_type = T;
	using is_always_equal = std::false_type;

	my_allocator() = default;
	my_allocator(const my_allocator&) = default;

	value_type* allocate(std::size_t n) {
		return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
	}
	void deallocate(value_type* p, std::size_t) {
		::operator delete(p);
	}
};

template <class T>
bool operator==(const my_allocator<T>& lhs, const my_allocator<T>& rhs) {
	return false;
}

template <class T>
bool operator!=(const my_allocator<T>& lhs, const my_allocator<T>& rhs) {
	return true;
}

using deque_t = va::deque<int>;
using deque2_t = va::deque<int, my_allocator<int>>;

TEST(DequeTests, ConstructorTests) {
	// Default constructor
	deque_t d1;
	ASSERT_TRUE(d1.empty());
	ASSERT_EQ(d1.capacity(), 0);

	std::allocator<int> alloc{};
	my_allocator<int> my_alloc{};

	// Allocator constructor
	deque_t d2(alloc);
	ASSERT_TRUE(d1.empty());
	ASSERT_EQ(d1.capacity(), 0);
	ASSERT_EQ(d2.get_allocator(), alloc);
	
	// n default initialized element constructor
	deque_t d3(N);
	ASSERT_EQ(d3.size(), N);
	ASSERT_EQ(d3.capacity(), N);
	
	deque_t d4(N, alloc);
	ASSERT_EQ(d4.size(), N);
	ASSERT_EQ(d4.capacity(), N);
	ASSERT_EQ(d4.get_allocator(), alloc);

	// n copy constructed element constructor
	deque_t d5(N, 10);
	ASSERT_EQ(d5.size(), N);
	ASSERT_EQ(d5.capacity(), N);
	for (auto i : d5) ASSERT_EQ(i, 10);

	deque_t d6(N, 10, alloc);
	ASSERT_EQ(d6.size(), N);
	ASSERT_EQ(d6.capacity(), N);
	for (auto i : d6) ASSERT_EQ(i, 10);
	ASSERT_EQ(d6.get_allocator(), alloc);

	// Range based constructor
	std::deque<int> deque{ 1, 2, 3, 4, 5 };
	deque_t d7(deque.begin(), deque.end());
	ASSERT_TRUE(std::equal(d7.begin(), d7.end(), deque.begin(), deque.end()));

	deque_t d8(deque.begin(), deque.end(), alloc);
	ASSERT_TRUE(std::equal(d8.begin(), d8.end(), deque.begin(), deque.end()));

	// Copy constructor
	deque_t d9(d8);
	ASSERT_TRUE(std::equal(d9.begin(), d9.end(), d8.begin(), d8.end()));

	deque_t d10(d9, alloc);
	ASSERT_TRUE(std::equal(d10.begin(), d10.end(), d9.begin(), d9.end()));
	ASSERT_EQ(d10.get_allocator(), alloc);

	// Move constructor
	deque_t d11(std::move(d10));
	ASSERT_TRUE(d10.empty());
	ASSERT_TRUE(std::equal(d11.begin(), d11.end(), d9.begin(), d9.end()));

	deque_t d12(std::move(d11), alloc);
	ASSERT_TRUE(d11.empty());
	ASSERT_TRUE(std::equal(d12.begin(), d12.end(), d9.begin(), d9.end()));

	std::initializer_list<int> list = { 1, 2, 3, 4, 5 };

	deque2_t d13(list.begin(), list.end());
	deque2_t d14(std::move(d13));
	ASSERT_TRUE(d13.empty());
	ASSERT_TRUE(std::equal(d14.begin(), d14.end(), list.begin(), list.end()));

	deque2_t d15(std::move(d14), my_alloc);
	ASSERT_EQ(d14.size(), list.size());
	ASSERT_TRUE(std::equal(d15.begin(), d15.end(), list.begin(), list.end()));

	deque_t d16(list);
	ASSERT_EQ(d16.size(), list.size());
	ASSERT_TRUE(std::equal(d16.begin(), d16.end(), list.begin(), list.end()));
}

TEST(DequeTests, AssignmentTests) {
	// Copy assignment
	deque_t d1({ 1, 2, 3, 4, 5 });
	deque_t d2({ 5, 6, 7, 8, 9 });
	ASSERT_FALSE(std::equal(d2.begin(), d2.end(), d1.begin(), d1.end()));
	d2 = d1;
	ASSERT_TRUE(std::equal(d2.begin(), d2.end(), d1.begin(), d1.end()));

	deque2_t d3({ 1, 2, 3, 4 });
	deque2_t d4({ 5, 6, 7, 8 });
	ASSERT_FALSE(std::equal(d3.begin(), d3.end(), d4.begin(), d4.end()));
	d3 = d4;
	ASSERT_TRUE(std::equal(d3.begin(), d3.end(), d4.begin(), d4.end()));

	// Move assignment
	deque_t d5({ 1, 2, 3, 4 });
	deque_t d6({ 5, 6, 7, 8 });
	deque_t d7({ 5, 6, 7, 8 });
	ASSERT_FALSE(std::equal(d5.begin(), d5.end(), d6.begin(), d6.end()));
	d5 = std::move(d6);
	ASSERT_TRUE(d6.empty());
	ASSERT_TRUE(std::equal(d5.begin(), d5.end(), d7.begin(), d7.end()));

	deque2_t d8({ 1, 2, 3, 4 });
	deque2_t d9({ 5, 6, 7, 8 });
	deque2_t d10({ 5, 6, 7, 8 });
	ASSERT_FALSE(std::equal(d8.begin(), d8.end(), d9.begin(), d9.end()));
	d8 = std::move(d9);
	ASSERT_FALSE(d9.empty());
	ASSERT_TRUE(std::equal(d8.begin(), d8.end(), d10.begin(), d10.end()));

	// Assign member function
	deque_t d11;
	d11.assign(5, 10);
	ASSERT_EQ(d11.size(), 5);
	for (auto i : d11) ASSERT_EQ(i, 10);

	d11.assign({ 7, 7, 7, 7 });
	ASSERT_EQ(d11.size(), 4);
	for (auto i : d11) ASSERT_EQ(i, 7);
}

TEST(DequeTests, ElementAccessTests) {
	deque_t d1({ 1, 2, 3, 4, 5 });
	ASSERT_EQ(d1.front(), 1);
	ASSERT_EQ(d1.back(), 5);
	ASSERT_EQ(d1[0], 1);
	ASSERT_EQ(d1[1], 2);
	ASSERT_EQ(d1[2], 3);
	ASSERT_EQ(d1[3], 4);
	ASSERT_EQ(d1[4], 5);

	d1.push_front(0);
	ASSERT_EQ(d1.size(), 6);
	ASSERT_EQ(d1.front(), 0);
	ASSERT_EQ(d1[0], 0);

	d1.push_back(6);
	ASSERT_EQ(d1.size(), 7);
	ASSERT_EQ(d1.back(), 6);
	ASSERT_EQ(d1[6], 6);
}

TEST(DequeTests, IteratorTests) {
	deque_t deque({ 0, 1, 2, 3, 4, 5, 6 });

	ASSERT_TRUE(deque.begin() < deque.end());
	ASSERT_TRUE(deque.end() > deque.begin());
	ASSERT_TRUE(deque.begin() == deque.begin());
	ASSERT_TRUE(deque.begin() <= deque.end());
	ASSERT_TRUE(deque.end() >= deque.begin());

	ASSERT_EQ(deque.end() - deque.begin(), 7);
	deque.push_front(-1);
	deque.push_back(7);
	ASSERT_EQ(deque.end() - deque.begin(), 9);

	auto it = deque.begin() + 3;
	ASSERT_EQ(*it, 2);

	it = deque.end() - 4;
	ASSERT_EQ(*it, 4);

	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	std::shuffle(deque.begin(), deque.end(), std::mt19937{ std::random_device{}() });
	ASSERT_FALSE(std::is_sorted(deque.begin(), deque.end()));
	std::sort(deque.begin(), deque.end());
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
}

TEST(DequeTests, CapacityTests) {
	deque_t deque;
	for (int i = 0; i < N; ++i)
		deque.push_back(i);
	ASSERT_EQ(deque.size(), N);
	ASSERT_NE(deque.capacity(), deque.size());
	deque.shrink_to_fit();
	ASSERT_EQ(deque.capacity(), deque.size());
	deque.clear();
	ASSERT_TRUE(deque.empty());
	ASSERT_EQ(deque.max_size(), std::allocator_traits<std::allocator<int>>::max_size(std::allocator<int>{}));
}

TEST(DequeTests, InsertionTests) {
	std::vector<int> vector{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	deque_t deque({ 2, 4, 6, 8, 10 });

	auto it = deque.insert(deque.begin(), 1);
	ASSERT_EQ(*it, 1);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [1, 2, 4, 6, 8, 10]

	it = deque.insert(deque.end(), 11);
	ASSERT_EQ(*it, 11);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [1, 2, 4, 6, 8, 10, 11]

	it = deque.insert(deque.begin() + 2, 3);
	ASSERT_EQ(*it, 3);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [1, 2, 3, 4, 6, 8, 10, 11]

	it = deque.insert(deque.begin() + 4, 5);
	ASSERT_EQ(*it, 5);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [1, 2, 3, 4, 5, 6, 8, 10, 11]

	it = deque.insert(deque.begin() + 6, 7);
	ASSERT_EQ(*it, 7);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [1, 2, 3, 4, 5, 6, 7, 8, 10, 11]

	it = deque.insert(deque.begin() + 8, 9);
	ASSERT_EQ(*it, 9);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));

	ASSERT_TRUE(std::equal(deque.begin(), deque.end(), vector.begin(), vector.end()));

	deque.clear();
	it = deque.insert(deque.begin(), 5, 10);
	ASSERT_EQ(*it, 10);
	ASSERT_EQ(deque.size(), 5);
	for (auto i : deque) ASSERT_EQ(i, 10);

	std::initializer_list<int> list = { 4, 5, 6, 7 };
	deque = { 1, 2, 3, 8, 9, 10, 11 };
	it = deque.insert(deque.begin() + 3, list);
	ASSERT_EQ(*it, 4);
	ASSERT_TRUE(std::equal(deque.begin(), deque.end(), vector.begin(), vector.end()));

}

TEST(DequeTests, ErasureTests) {
	std::vector<int> vector{ 2, 4, 6, 8, 10 };
	deque_t deque{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

	auto it = deque.erase(deque.begin());
	ASSERT_EQ(*it, 2);
	ASSERT_EQ(deque.size(), 10);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [2, 3, 4, 5, 6, 7, 8, 9, 10, 11]

	it = deque.erase(std::prev(deque.end()));
	ASSERT_EQ(it, deque.end());
	ASSERT_EQ(deque.size(), 9);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [2, 3, 4, 5, 6, 7, 8, 9, 10]

	it = deque.erase(deque.begin() + 1);
	ASSERT_EQ(*it, 4);
	ASSERT_EQ(deque.size(), 8);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [2, 4, 5, 6, 7, 8, 9, 10]

	it = deque.erase(deque.begin() + 2);
	ASSERT_EQ(*it, 6);
	ASSERT_EQ(deque.size(), 7);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [2, 4, 6, 7, 8, 9, 10]

	it = deque.erase(deque.begin() + 5);
	ASSERT_EQ(*it, 10);
	ASSERT_EQ(deque.size(), 6);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [2, 4, 6, 7, 8, 10]

	it = deque.erase(deque.begin() + 3);
	ASSERT_EQ(*it, 8);
	ASSERT_EQ(deque.size(), 5);
	ASSERT_TRUE(std::is_sorted(deque.begin(), deque.end()));
	// [2, 4, 6, 7, 8, 10]

	ASSERT_TRUE(std::equal(deque.begin(), deque.end(), vector.begin(), vector.end()));

	deque = { 1, 2, 3, 4, 5, 6 };
	it = deque.erase(deque.begin() + 3, deque.end());
	ASSERT_EQ(deque.size(), 3);
	ASSERT_EQ(deque.front(), 1);
	ASSERT_EQ(deque.back(), 3);
	ASSERT_EQ(deque[0], 1);
	ASSERT_EQ(deque[1], 2);
	ASSERT_EQ(deque[2], 3);
	
	deque = { 1, 2, 3, 4, 5, 6 };
	it = deque.erase(deque.begin(), deque.begin() + 3);
	ASSERT_EQ(deque.size(), 3);
	ASSERT_EQ(deque.front(), 4);
	ASSERT_EQ(deque.back(), 6);
	ASSERT_EQ(deque[0], 4);
	ASSERT_EQ(deque[1], 5);
	ASSERT_EQ(deque[2], 6);
}

TEST(DequeTests, ResizingTests) {
	deque_t deque{1, 2, 3, 4, 5};

	deque.resize(6);
	ASSERT_EQ(deque.size(), 6);

	deque.resize(4);
	ASSERT_EQ(deque.size(), 4);

	deque.resize(9, 5);
	ASSERT_EQ(deque.size(), 9);

	deque.resize(3, 11);
	ASSERT_EQ(deque.size(), 3);
	ASSERT_EQ(deque[0], 1);
	ASSERT_EQ(deque[1], 2);
	ASSERT_EQ(deque[2], 3);
}

TEST(DequeTests, SwapTests) {
	va::deque<int> d1 = { 1, 2, 3, 4 };
	va::deque<int> d2 = { 5, 6, 7, 8 };
	
	std::swap(d1, d2);
	
	ASSERT_EQ(d1, va::deque<int>({5, 6, 7, 8}));
	ASSERT_EQ(d2, va::deque<int>({1, 2, 3, 4}));
	
	va::deque<int, my_allocator<int>> d3 = { 1, 2, 3, 4 };
	va::deque<int, my_allocator<int>> d4 = { 5, 6, 7, 8 };

	std::swap(d3, d4);

	// No change since my_allocator does not propagate on swap or is always equal

	va::deque<int, my_allocator<int>> y{ 1, 2, 3, 4 };
	va::deque<int, my_allocator<int>> z{ 5, 6, 7, 8 };

	ASSERT_EQ(d3, y);
	ASSERT_EQ(d4, z);

}
