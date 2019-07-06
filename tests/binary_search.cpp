#include <gtest/gtest.h>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include "detail/constants.hpp"
#include "../include/libra/algorithm/binary_search.hpp"

TEST(BinarySearchTests, LowerBoundTest) {
	std::vector<int> nums;
	std::srand(std::time(nullptr));
	std::generate_n(std::back_inserter(nums), N, []() { return std::rand() % 100 + 1; });
	std::sort(nums.begin(), nums.end());
	auto std_lower = std::lower_bound(nums.begin(), nums.end(), nums[nums.size() / 2]);
	auto lower = libra::lower_bound(nums.begin(), nums.end(), nums[nums.size() / 2]);
	ASSERT_EQ(std_lower, lower);
	for (auto i = 0; i != N; ++i) {
		std::generate(nums.begin(), nums.end(), []() { return std::rand() % 100 + 1;  });
		auto idx = std::rand() % nums.size();
		auto std_lower = std::lower_bound(nums.begin(), nums.end(), nums[idx]);
		auto lower = libra::lower_bound(nums.begin(), nums.end(), nums[idx]);
		ASSERT_EQ(std_lower, lower);
	}
}

TEST(BinarySearchTests, UpperBoundTest) {
	std::vector<int> nums;
	std::srand(std::time(nullptr));
	std::generate_n(std::back_inserter(nums), N, []() { return std::rand() % 100 + 1; });
	std::sort(nums.begin(), nums.end());
	auto std_upper = std::upper_bound(nums.begin(), nums.end(), nums[nums.size() / 2]);
	auto upper = libra::upper_bound(nums.begin(), nums.end(), nums[nums.size() / 2]);
	ASSERT_EQ(std_upper, upper);
	for (auto i = 0; i != N; ++i) {
		std::generate(nums.begin(), nums.end(), []() { return std::rand() % 100 + 1;  });
		auto idx = std::rand() % nums.size();
		std_upper = std::upper_bound(nums.begin(), nums.end(), nums[idx]);
		upper = libra::upper_bound(nums.begin(), nums.end(), nums[idx]);
		ASSERT_EQ(std_upper, upper);
	}
}

TEST(BinarySearchTests, BinarySearchTest) {
	std::vector<int> nums;
	std::srand(std::time(nullptr));
	std::generate_n(std::back_inserter(nums), N, []() { return std::rand() % 100 + 1; });
	std::sort(nums.begin(), nums.end());
	auto std_bs = std::binary_search(nums.begin(), nums.end(), nums[nums.size() / 2]);
	auto bs = libra::binary_search(nums.begin(), nums.end(), nums[nums.size() / 2]);
	ASSERT_EQ(std_bs, bs);
	for (auto i = 0; i != N; ++i) {
		std::generate(nums.begin(), nums.end(), []() { return std::rand() % 100 + 1;  });
		auto idx = std::rand() % nums.size();
		std_bs = std::binary_search(nums.begin(), nums.end(), nums[idx]);
		bs = libra::binary_search(nums.begin(), nums.end(), nums[idx]);
		ASSERT_EQ(std_bs, bs);
	}
}

TEST(BinarySearchTests, EqualRangeTests) {
	std::vector<int> nums;
	std::srand(std::time(nullptr));
	std::generate_n(std::back_inserter(nums), N, []() { return std::rand() % 100 + 1; });
	std::sort(nums.begin(), nums.end());
	auto std_eq_rng = std::equal_range(nums.begin(), nums.end(), nums[nums.size() / 2]);
	auto eq_rng = libra::equal_range(nums.begin(), nums.end(), nums[nums.size() / 2]);
	ASSERT_EQ(std_eq_rng.first, eq_rng.first);
	ASSERT_EQ(std_eq_rng.second, eq_rng.second);
	for (auto i = 0; i != N; ++i) {
		std::generate(nums.begin(), nums.end(), []() { return std::rand() % 100 + 1;  });
		auto idx = std::rand() % nums.size();
		std_eq_rng = std::equal_range(nums.begin(), nums.end(), nums[idx]);
		eq_rng = libra::equal_range(nums.begin(), nums.end(), nums[idx]);
		ASSERT_EQ(std_eq_rng.first, eq_rng.first);
		ASSERT_EQ(std_eq_rng.second, eq_rng.second);
	}
}