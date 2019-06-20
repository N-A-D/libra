#include <gtest/gtest.h>
#include "../include/libva/ordered_multimap.hpp"
#include "detail/constants.hpp"
#include <random>
#include <vector>
#include <algorithm>

std::mt19937 gen{ std::random_device{}() };

using multimap_type = va::ordered_multimap<int, int>;
using pair_type = typename multimap_type::value_type;

struct secondary_compare {
	bool operator()(const pair_type& lhs, const pair_type& rhs) const {
		return lhs.second < rhs.second;
	}
};

TEST(OrderedMultimapTests, ConstructorTests) {
	// Setup
	std::vector<pair_type> pairs(N);
	std::generate(pairs.begin(), pairs.end(), [n = 0]() mutable { return std::make_pair(0, n++); });

	// Test empty container
	multimap_type mm1;
	ASSERT_TRUE(mm1.empty());

	// Test order of insertion

	// Test construction from external container
	multimap_type mm2(pairs.begin(), pairs.end());
	ASSERT_EQ(N, mm2.count(0));
	ASSERT_TRUE(std::is_sorted(mm2.begin(), mm2.end(), mm2.value_comp()));
	ASSERT_TRUE(std::is_sorted(mm2.begin(), mm2.end(), secondary_compare()));

	// Test copy constructor
	multimap_type mm3(mm2);
	ASSERT_EQ(N, mm3.count(0));
	ASSERT_TRUE(std::is_sorted(mm3.begin(), mm3.end(), mm3.value_comp()));
	ASSERT_TRUE(std::is_sorted(mm3.begin(), mm3.end(), secondary_compare()));

	// Test move constructor
	multimap_type mm4(std::move(mm3));
	ASSERT_TRUE(mm3.empty());
	ASSERT_EQ(N, mm4.count(0));
	ASSERT_TRUE(std::is_sorted(mm4.begin(), mm4.end(), mm4.value_comp()));
	ASSERT_TRUE(std::is_sorted(mm4.begin(), mm4.end(), secondary_compare()));

	// Test container sortedness

	for (int i = 1; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			pairs.emplace_back(std::pair(i, j));
		}
	}

	// Test construction from external container
	multimap_type mm5(pairs.begin(), pairs.end());
	ASSERT_TRUE(std::is_sorted(mm5.begin(), mm5.end(), mm5.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, mm5.count(i));
	}

	// Test copy constructor
	multimap_type mm6(mm5);
	ASSERT_TRUE(std::is_sorted(mm6.begin(), mm6.end(), mm6.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, mm6.count(i));
	}

	// Test move constructor
	multimap_type mm7(std::move(mm6));
	ASSERT_TRUE(mm6.empty());
	ASSERT_TRUE(std::is_sorted(mm7.begin(), mm7.end(), mm7.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, mm7.count(i));
	}
}

TEST(OrderedMultimapTests, AssignmentTests) {
	// Setup
	std::vector<pair_type> pairs(N);
	std::generate(pairs.begin(), pairs.end(), [n = 0]() mutable { return std::make_pair(0, n++); });

	multimap_type mm1(pairs.begin(), pairs.end());
	ASSERT_EQ(N, mm1.count(0));
	ASSERT_TRUE(std::is_sorted(mm1.begin(), mm1.end(), mm1.value_comp()));
	ASSERT_TRUE(std::is_sorted(mm1.begin(), mm1.end(), secondary_compare()));

	// Test order of insertion

	// Test copy assignment
	multimap_type mm2 = mm1;
	ASSERT_TRUE(!mm1.empty() && mm1.size() == N);
	ASSERT_EQ(N, mm2.count(0));
	ASSERT_TRUE(std::is_sorted(mm2.begin(), mm2.end(), mm2.value_comp()));
	ASSERT_TRUE(std::is_sorted(mm2.begin(), mm2.end(), secondary_compare()));

	// Test move assignment
	multimap_type mm3 = std::move(mm2);
	ASSERT_TRUE(mm2.empty());
	ASSERT_EQ(N, mm3.count(0));
	ASSERT_TRUE(std::is_sorted(mm3.begin(), mm3.end(), mm3.value_comp()));
	ASSERT_TRUE(std::is_sorted(mm3.begin(), mm3.end(), secondary_compare()));

	// Test container sortedness
	for (int i = 1; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			pairs.emplace_back(std::pair(i, j));
		}
	}

	// Test copy assignment
	multimap_type mm4(pairs.begin(), pairs.end());

	multimap_type mm5 = mm4;
	ASSERT_TRUE(std::is_sorted(mm5.begin(), mm5.end(), mm5.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, mm5.count(i));
	}

	// test move assignment
	multimap_type mm6 = std::move(mm4);
	ASSERT_TRUE(mm4.empty());
	ASSERT_TRUE(std::is_sorted(mm6.begin(), mm6.end(), mm6.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, mm6.count(i));
	}
}

TEST(OrderedMultimapTests, InsertionTests) {
	// Setup
	std::srand(std::time(nullptr));
	std::vector<pair_type> pairs(N);
	std::generate(pairs.begin(), pairs.end(), [n = 0]() mutable { return std::make_pair(0, n++); });
	multimap_type multimap;

	// Test order of insertion

	// Test single value insert
	for (auto pair : pairs) {
		auto ret = multimap.insert(pair);
		ASSERT_EQ(pair.first, ret->first);
	}
	ASSERT_EQ(N, multimap.count(0));
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), multimap.value_comp()));
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), secondary_compare()));

	multimap.clear();

	// Test range based insert
	multimap.insert(pairs.begin(), pairs.end());
	ASSERT_EQ(N, multimap.count(0));
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), multimap.value_comp()));
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), secondary_compare()));

	multimap.clear();

	// Test random hint
	auto it = multimap.begin();
	for (auto pair : pairs) {
		auto ret = multimap.insert(it, pair);
		ASSERT_EQ(pair.first, ret->first);
		it = multimap.begin() + std::rand() % multimap.size();
	}
	ASSERT_EQ(N, multimap.count(0));
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), multimap.value_comp()));
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), secondary_compare()));

	multimap.clear();

	// Test container sortedness

	// Setup
	for (int i = 1; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			pairs.emplace_back(std::make_pair(i, j));
		}
	}
	std::shuffle(pairs.begin(), pairs.end(), gen);
	
	// Test random single value insert
	for (auto pair : pairs) {
		auto ret = multimap.insert(pair);
		ASSERT_EQ(pair.first, ret->first);
	}
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), multimap.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, multimap.count(i)); // This is what count does behind the scenes
	}

	multimap.clear();

	// Test random range based insert
	multimap.insert(pairs.begin(), pairs.end());
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), multimap.value_comp()));
	for (int i = 0; i < N; ++i) {
		auto range = multimap.equal_range(i);
		ASSERT_EQ(N, range.second - range.first); // This is what count does behind the scenes
	}

	multimap.clear();

	// Test random hint insertion
	it = multimap.begin();
	for (auto pair : pairs) {
		auto ret = multimap.insert(it, pair);
		ASSERT_EQ(pair.first, ret->first);
		it = multimap.begin() + std::rand() % multimap.size();
	}
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), multimap.value_comp()));
	for (int i = 0; i < N; ++i) {
		auto range = multimap.equal_range(i);
		ASSERT_EQ(N, range.second - range.first); // This is what count does behind the scenes
	}
}

TEST(OrderedMultimapTests, ErasureTests){
	// setup
	std::vector<pair_type> pairs;
	for (int i = 1; i <= N; ++i) {
		for (int j = 1; j <= N; ++j) {
			pairs.emplace_back(std::pair(i, j));
		}
	}
	std::shuffle(pairs.begin(), pairs.end(), gen);

	multimap_type multimap(pairs.begin(), pairs.end());
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), multimap.value_comp()));

	for (auto i = 1; i <= N; ++i) {
		ASSERT_EQ(N, multimap.erase(i));
	}
	
	ASSERT_TRUE(multimap.empty());
	multimap.insert(pairs.begin(), pairs.end());
	for (int i = 1; i <= N; ++i) {
		int count = 0;
		while (multimap.contains(i)) {
			auto it = multimap.find(i);
			multimap.erase(it);
			++count;
		}
		ASSERT_EQ(N, count);
	}
}

TEST(OrderedMultimapTests, LookupTests) {
	// setup
	std::vector<pair_type> pairs;
	for (int i = 1; i <= N; ++i) {
		for (int j = 1; j <= N; ++j) {
			pairs.emplace_back(std::pair(i, j));
		}
	}
	std::shuffle(pairs.begin(), pairs.end(), gen);

	multimap_type multimap(pairs.begin(), pairs.end());
	ASSERT_TRUE(std::is_sorted(multimap.begin(), multimap.end(), multimap.value_comp()));

	for (int i = 1; i <= N; ++i) {
		ASSERT_FALSE(multimap.contains(-i));
		ASSERT_EQ(0, multimap.count(-i));
		ASSERT_TRUE(multimap.contains(i));
		ASSERT_EQ(N, multimap.count(i));
	}
}

TEST(OrderedMultimapTests, LexicographicalTests) {
	ASSERT_EQ(multimap_type({ {0, 0}, {1, 1}, {2, 2} }), multimap_type({ {0, 0}, {1, 1}, {2, 2} }));
	ASSERT_LE(multimap_type({ {0, 0}, {1, 1}, {2, 2} }), multimap_type({ {1, 2}, {2, 5} }));
	ASSERT_GE(multimap_type({ {0, 0}, {1, 1}, {2, 2} }), multimap_type({ {0, 0}, {1, 1} }));
}

TEST(OrderedMultimapTests, SwapTests) {
	multimap_type mm1({ {0, 0}, {1, 1}, {2, 2} });
	multimap_type mm2({ {0, 0}, {1, 1}, {2, 2} });
	mm1.swap(mm2);
	ASSERT_EQ(mm2, multimap_type({ {0, 0}, {1, 1}, {2, 2} }));
	ASSERT_EQ(mm1, multimap_type({ {0, 0}, {1, 1}, {2, 2} }));
}