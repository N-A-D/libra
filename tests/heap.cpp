#include <gtest/gtest.h>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include "detail/constants.hpp"
#include "../include/libra/algorithm/heap.hpp"

TEST(HeapTests, MakeHeapTests) {
	std::vector<int> nums;
	std::srand(std::time(nullptr));
	std::generate_n(std::back_inserter(nums), N, []() { return std::rand() % 100 + 1; });
	libra::make_heap(nums.begin(), nums.end());
	ASSERT_TRUE(std::is_heap(nums.begin(), nums.end()));
	for (int i = 0; i != N; ++i) {
		std::generate(nums.begin(), nums.end(), []() { return std::rand() % 100 + 1; });
		libra::make_heap(nums.begin(), nums.end());
		ASSERT_TRUE(std::is_heap(nums.begin(), nums.end()));
	}
}

TEST(HeapTests, PushHeapTests) {
	std::vector<int> nums;
	std::srand(std::time(nullptr));
	std::generate_n(std::back_inserter(nums), N, []() { return std::rand() % 100 + 1; });
	libra::make_heap(nums.begin(), nums.end());
	ASSERT_TRUE(std::is_heap(nums.begin(), nums.end()));
	for (int i = 0; i != N; ++i) {
		nums.emplace_back(std::rand() % 100 + 1);
		libra::push_heap(nums.begin(), nums.end());
		ASSERT_TRUE(std::is_heap(nums.begin(), nums.end()));
	}
}

TEST(HeapTests, PopHeapTests) {
	std::vector<int> nums;
	std::srand(std::time(nullptr));
	std::generate_n(std::back_inserter(nums), N, []() { return std::rand() % 100 + 1; });
	libra::make_heap(nums.begin(), nums.end());
	ASSERT_TRUE(std::is_heap(nums.begin(), nums.end()));
	for (int i = 0; i != N; ++i) {
		libra::pop_heap(nums.begin(), nums.end());
		nums.pop_back();
		ASSERT_TRUE(std::is_heap(nums.begin(), nums.end()));
		std::generate(nums.begin(), nums.end(), []() { return std::rand() % 100 + 1; });
	}
}