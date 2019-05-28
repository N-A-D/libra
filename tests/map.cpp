#include <gtest/gtest.h>
#include "../include/libva/ordered_map.hpp"
#include "detail/constants.hpp"
#include <random>
#include <vector>
#include <algorithm>

using map_type = va::ordered_map<int, int>;
using pair_type = std::pair<int, int>;

struct equivalent {
	map_type::value_compare comp;
	equivalent(map_type::value_compare comp)
		: comp(comp) {}
	bool operator()(const map_type::value_type& lhs, const map_type::value_type& rhs) const {
		return !comp(lhs, rhs) && !comp(rhs, lhs);
	}
};

std::mt19937 gen{ std::random_device{}() };

TEST(OrderedMapTests, ConstructorTests) {
	map_type m1;
	ASSERT_TRUE(m1.empty());

	std::vector<pair_type> pairs(N);
	std::generate(pairs.begin(), pairs.end(), [n = 0]() mutable { 
		auto value = n++;
		return std::make_pair(value, value);
	});
	std::shuffle(pairs.begin(), pairs.end(), gen);

	// Test constructor from external container
	// Tests initializer list constructors as those forward to this one.
	map_type m2(pairs.begin(), pairs.end());
	ASSERT_EQ(pairs.size(), m2.size());
	ASSERT_TRUE(std::is_sorted(m2.begin(), m2.end(), m2.value_comp()));
	ASSERT_EQ(std::unique(m2.begin(), m2.end(), equivalent(m2.value_comp())), m2.end());

	// Tests construction from external container with duplicates
	size_t size = pairs.size();
	auto copy(pairs);
	for (auto it = copy.begin(); it != copy.end(); ++it)
		pairs.emplace_back(*it);
	std::shuffle(pairs.begin(), pairs.end(), gen);
	
	map_type m3(pairs.begin(), pairs.end());
	ASSERT_EQ(N, m3.size());
	ASSERT_TRUE(std::is_sorted(m3.begin(), m3.end(), m3.value_comp()));
	ASSERT_EQ(std::unique(m3.begin(), m3.end(), equivalent(m3.value_comp())), m3.end());

	// Test copy construction
	map_type copier(m3);
	ASSERT_EQ(N, copier.size());
	ASSERT_TRUE(std::is_sorted(copier.begin(), copier.end(), copier.value_comp()));
	ASSERT_EQ(std::unique(copier.begin(), copier.end(), equivalent(copier.value_comp())), copier.end());

	// Test move construction
	map_type thief(std::move(copier));
	ASSERT_TRUE(copier.empty());
	ASSERT_TRUE(std::is_sorted(thief.begin(), thief.end(), thief.value_comp()));
	ASSERT_EQ(std::unique(thief.begin(), thief.end(), equivalent(thief.value_comp())), thief.end());
}

TEST(OrderedMapTests, AssignmentTests) {
	std::vector<pair_type> pairs(N);
	std::generate(pairs.begin(), pairs.end(), [n = 0]() mutable { 
		auto value = n++;
		return std::make_pair(value, value);
	});
	std::shuffle(pairs.begin(), pairs.end(), gen);

	// Test copy assignment
	map_type m1(pairs.begin(), pairs.end());
	map_type m2 = m1;
	ASSERT_EQ(N, m1.size());
	ASSERT_EQ(m1.size(), m2.size());
	ASSERT_TRUE(std::is_sorted(m2.begin(), m2.end(), m2.value_comp()));
	ASSERT_EQ(std::unique(m2.begin(), m2.end(), equivalent(m2.value_comp())), m2.end());

	// Test move assignment
	map_type m3 = std::move(m2);
	ASSERT_TRUE(m2.empty());
	ASSERT_EQ(m1.size(), m3.size());
	ASSERT_TRUE(std::is_sorted(m3.begin(), m3.end(), m3.value_comp()));
	ASSERT_EQ(std::unique(m3.begin(), m3.end(), equivalent(m3.value_comp())), m3.end());

	// Test initializer list assignment
	std::initializer_list<pair_type> list{ {1, 1}, {2, 2}, {3, 3}, {4, 4} };
	m3 = list;
	ASSERT_EQ(list.size(), m3.size());
	ASSERT_TRUE(std::is_sorted(m3.begin(), m3.end(), m3.value_comp()));
	ASSERT_EQ(std::unique(m3.begin(), m3.end(), equivalent(m3.value_comp())), m3.end());
}

TEST(OrderedMapTests, InsertionTests) {
	map_type map;

	// Test insertion
	for (int i = 0; i < N; ++i) {
		auto ret = map.insert(map_type::value_type(i, i));
		ASSERT_TRUE(ret.second);
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end(), map.value_comp()));
	ASSERT_EQ(std::unique(map.begin(), map.end(), equivalent(map.value_comp())), map.end());

	map.clear();

	// Test insertion with correct hint
	auto it = map.begin();
	for (int i = 0; i < N; ++i) {
		auto value = std::make_pair(i, i);
		map.insert(it, value);
		it = map.end();
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();

	// Test insertion with incorrect hint
	it = map.begin();
	for (int i = N; i > 0; --i) {
		auto value = std::make_pair(i, i);
		map.insert(it, value);
		it = map.end();
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();

	// Test insertion with corrected hint
	it = map.begin();
	for (int i = N; i > 0; --i) {
		auto value = std::make_pair(i, i);
		map.insert(it, value);
		it = map.begin();
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();

	// Test insertion with close-by hints
	it = map.begin();
	for (int i = 0; i < N; ++i) {
		auto value = std::make_pair(i, i);
		it = map.insert(it, value);
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();


	// Setup random integers
	std::srand(std::time(nullptr));
	std::vector<int> integers(N);
	std::generate(integers.begin(), integers.end(), [n = 0]() mutable { return n++; });
	std::shuffle(integers.begin(), integers.end(), gen);

	// Test random integer insertion
	for (auto integer : integers) {
		auto ret = map.insert(std::make_pair(integer, integer));
		ASSERT_TRUE(ret.second);
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();

	// Test random hint insertion
	it = map.begin();
	for (auto integer : integers) {
		auto value = std::make_pair(integer, integer);
		it = map.insert(it, value);
		it = map.begin() + std::rand() % map.size();
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();

	// Duplicate value mapup
	auto copy = integers;
	std::copy(copy.begin(), copy.end(), std::back_inserter(integers));
	std::shuffle(integers.begin(), integers.end(), gen);

	// Test random insertion with duplicates
	for (auto integer : integers) {
		map.insert(std::make_pair(integer, integer));
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());

	map.clear();

	// Test random hinted insertion with duplicates
	it = map.begin();
	for (auto integer : integers) {
		auto value = std::make_pair(integer, integer);
		map.insert(it, value);
		it = map.begin() + std::rand() % map.size();
	}
	ASSERT_EQ(N, map.size());
	ASSERT_TRUE(std::is_sorted(map.begin(), map.end()));
	ASSERT_EQ(std::unique(map.begin(), map.end()), map.end());
}

TEST(OrderedMapTests, ErasureTests) {
	map_type map;
	std::vector<int> integers;
	for (int i = 0; i < N; ++i) {
		integers.emplace_back(i);
		auto value = map_type::value_type(i, i);
		map.insert(map.end(), value);
	}
	for (auto integer : integers) {
		ASSERT_EQ(1, map.erase(integer));
	}
	ASSERT_TRUE(map.empty());
	for (auto integer : integers) {
		map.insert(map_type::value_type(integer, integer));
	}
	for (auto integer : integers) {
		auto it = map.find(integer);
		map.erase(it);
		ASSERT_FALSE(map.contains(integer));
	}
}

TEST(OrderedMapTests, LookupTests) {
	map_type map;
	std::vector<int> integers;
	for (int i = 1; i <= N; ++i) {
		integers.emplace_back(i);
		auto value = map_type::value_type(i, i);
		map.insert(map.end(), value);
	}
	std::shuffle(integers.begin(), integers.end(), gen);
	for (auto integer : integers) {
		ASSERT_FALSE(map.contains(-integer));
		ASSERT_EQ(0, map.count(-integer));
		ASSERT_TRUE(map.contains(integer));
		ASSERT_TRUE(1, map.count(integer));
	}
}

TEST(OrderedMapTests, LexicographicalTests) {
	ASSERT_EQ(map_type({ {1, 1}, {2, 2}, {3, 3} }), map_type({ {1, 1}, {2, 2}, {3, 3} }));
	ASSERT_LE(map_type({ {1, 1}, {2, 2}, {3, 3} }), map_type({ {2, 3}, {3, 8}, {4, 3} }));
}

TEST(OrderedMapTests, SwapTest) {
	map_type m1({ {1, 1}, {2, 2}, {3, 3} });
	map_type m2({ {2, 3}, {3, 8}, {4, 3} });
	m1.swap(m2);
	ASSERT_EQ(m1, map_type({ {2, 3}, {3, 8}, {4, 3} }));
	ASSERT_EQ(m2, map_type({ {1, 1}, {2, 2}, {3, 3} }));
}