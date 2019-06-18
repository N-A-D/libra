#include <gtest/gtest.h>
#include <deque>
#include <vector>
#include <random>
#include <algorithm>
#include "detail/constants.hpp"
#include "../include/libva/deque.hpp"

TEST(DequeTests, ConstructorTests) {
	// default constructor
	va::deque<int> d1;
	ASSERT_TRUE(d1.empty());

	// allocator supplied constructor
	va::deque<int> d2(std::allocator<int>{});
	ASSERT_TRUE(d1.empty());

	// n default constructed element constructor
	va::deque<int> d3(N);
	ASSERT_EQ(d3.size(), N);

	// n copy constructed element constructor
	va::deque<int> d4(N, 1);
	ASSERT_EQ(d4.size(), N);
	for (auto i : d4) ASSERT_EQ(i, 1);

	// Range constructor
	std::srand(std::time(nullptr));
	std::deque<int> deque;
	std::generate_n(std::back_inserter(deque), N, []() mutable { return std::rand() % N + 1; });
	va::deque<int> d5(deque.begin(), deque.end());
	ASSERT_TRUE(std::equal(d5.begin(), d5.end(), deque.begin(), deque.end()));

	// copy constructor
	va::deque<int> d6(d5);
	ASSERT_TRUE(d6.size() == d5.size());
	ASSERT_TRUE(std::equal(d6.begin(), d6.end(), d5.begin(), d5.end()));

	// move constructor
	va::deque<int> d7(std::move(d6));
	ASSERT_TRUE(d6.empty());
	ASSERT_EQ(d7.size(), d5.size());
	ASSERT_TRUE(std::equal(d7.begin(), d7.end(), d5.begin(), d5.end()));
}

TEST(DequeTests, AssignmentTests) {
	// Missing is copy/move assignment

	std::initializer_list<int> list = { 1, 2, 3, 4, 5 };
	va::deque<int> d1 = list;
	ASSERT_TRUE(std::equal(d1.begin(), d1.end(), list.begin(), list.end()));

	list = { 1, 1, 1, 1 };
	va::deque<int> d2;
	d2.assign(list.size(), 1);
	ASSERT_TRUE(std::equal(d2.begin(), d2.end(), list.begin(), list.end()));

	va::deque<int> d3;
	d3.assign(list);
	ASSERT_TRUE(std::equal(d3.begin(), d3.end(), list.begin(), list.end()));
}

TEST(DequeTests, ElementAccessTests) {
	va::deque<int> d1;
	std::generate_n(std::back_inserter(d1), N, [n = 0]() mutable { return ++n; });
	ASSERT_EQ(d1.size(), N);
	ASSERT_EQ(d1.front(), 1);
	ASSERT_EQ(d1.back(), N);
	int x = 0;
	for (size_t i = 0; i != d1.size(); ++i)
		ASSERT_EQ(d1[i], ++x);
	for (size_t i = d1.size() - 1; i != -1; --i)
		ASSERT_EQ(d1[i], x--);

	va::deque<int> d2;
	std::generate_n(std::front_inserter(d2), N, [n = N]() mutable { return n--; });
	ASSERT_EQ(d2.size(), N);
	ASSERT_EQ(d2.front(), 1);
	ASSERT_EQ(d2.back(), N);
	x = 0;
	for (size_t i = 0; i < d2.size(); ++i)
		ASSERT_EQ(d2[i], ++x);
	for (size_t i = d2.size() - 1; i != -1; --i)
		ASSERT_EQ(d2[i], x--);
}

TEST(DequeTests, IteratorTests) {
	
}

TEST(DequeTests, CapacityTests) {

}

TEST(DequeTests, InsertionTests) {

}

TEST(DequeTests, ErasureTests) {

}

TEST(DequeTests, ResizeTests) {

}

TEST(DequeTests, SwapTests) {

}