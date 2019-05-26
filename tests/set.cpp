#include <gtest/gtest.h>
#include "../include/libva/ordered_set.hpp"
#include "utility.hpp"

using set_type = va::ordered_set<int>;

TEST(OrderedSetTests, ConstructorTests) {
	set_type s1;
	ASSERT_TRUE(s1.empty());

	std::vector<int> integers;
	for (int i = 0; i < N; ++i) {
		integers.emplace_back(i);
	}
	std::shuffle(integers.begin(), integers.end(), gen);

	// Tests construction from an external container
	set_type s2(integers.begin(), integers.end());
	ASSERT_EQ(s2.size(), integers.size());
	ASSERT_TRUE(std::is_sorted(s2.begin(), s2.end()));
	ASSERT_TRUE(is_unique(s2.begin(), s2.end()));

	// Tests construction from initializer list
	std::initializer_list<int> list{ 10, 5, 9, 4, 8, 3, 7, 2, 1, 6, 0, 22, 11 };
	set_type s3(list);
	ASSERT_EQ(list.size(), s3.size());
	ASSERT_TRUE(std::is_sorted(s3.begin(), s3.end()));
	ASSERT_TRUE(is_unique(s3.begin(), s3.end()));

	// Test construction from external container with duplicates
	size_t size = integers.size();
	for (size_t i = 0; i < size; ++i) {
		integers.emplace_back(static_cast<int>(i));
	}
	std::shuffle(integers.begin(), integers.end(), gen);
	set_type s4(integers.begin(), integers.end());
	ASSERT_EQ(size, s4.size());
	ASSERT_TRUE(std::is_sorted(s4.begin(), s4.end()));
	ASSERT_TRUE(is_unique(s4.begin(), s4.end()));
	
	// Test construction from initializer list with duplicates
	std::initializer_list<int> list2{ 10, 5, 9, 4, 8, 3, 7, 2, 1, 6, 0, 22, 11, 10, 5, 9, 4, 8, 3, 7, 2, 1, 6, 0, 22, 11 };
	set_type s5(list2);
	ASSERT_EQ(list.size(), s5.size());
	ASSERT_TRUE(std::is_sorted(s5.begin(), s5.end()));
	ASSERT_TRUE(is_unique(s5.begin(), s5.end()));

	// Test copy construction
	set_type copier(s2);
	ASSERT_EQ(s2.size(), copier.size());
	ASSERT_TRUE(std::is_sorted(copier.begin(), copier.end()));
	ASSERT_TRUE(is_unique(copier.begin(), copier.end()));

	// Test move construction
	set_type thief(std::move(copier));
	ASSERT_TRUE(copier.empty());
	ASSERT_TRUE(std::is_sorted(thief.begin(), thief.end()));
	ASSERT_TRUE(is_unique(thief.begin(), thief.end()));

}

TEST(OrderedSetTests, AssignmentTests) {
	std::vector<int> integers;
	for (int i = 0; i < N; ++i) {
		integers.emplace_back(i);
	}
	std::shuffle(integers.begin(), integers.end(), gen);

	// Test copy assignment
	set_type s1(integers.begin(), integers.end());
	set_type s2 = s1;
	ASSERT_EQ(s1.size(), s2.size());
	ASSERT_TRUE(std::is_sorted(s2.begin(), s2.end()));
	ASSERT_TRUE(is_unique(s2.begin(), s2.end()));

	// Test move assignment
	set_type s3 = std::move(s2);
	ASSERT_TRUE(s2.empty());
	ASSERT_EQ(s1.size(), s3.size());
	ASSERT_TRUE(std::is_sorted(s3.begin(), s3.end()));
	ASSERT_TRUE(is_unique(s3.begin(), s3.end()));

	// Test initializer_list assignment
	std::initializer_list<int> list{ 4,7,2,1,6,5,3 };
	s3 = list;
	ASSERT_EQ(list.size(), s3.size());
	ASSERT_TRUE(std::is_sorted(s3.begin(), s3.end()));
	ASSERT_TRUE(is_unique(s3.begin(), s3.end()));
}

TEST(OrderedSetTests, InsertionTests) {
	set_type set;

	// Test insertion
	for (int i = 0; i < N; ++i) {
		auto ret = set.insert(i);
		ASSERT_TRUE(ret.second);
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));

	set.clear();

	// Test insertion with correct hint
	auto it = set.begin();
	for (int i = 0; i < N; ++i) {
		set.insert(it, i);
		it = set.end();
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));

	set.clear();

	// Test insertion with incorrect hint
	it = set.begin();
	for (int i = N; i > 0; --i) {
		set.insert(it, i);
		it = set.end();
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));

	set.clear();

	// Test insertion with corrected hint
	it = set.begin();
	for (int i = N; i > 0; --i) {
		set.insert(it, i);
		it = set.begin();
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));

	set.clear();

	// Test insertion with close-by hints
	it = set.begin();
	for (int i = 0; i < N; ++i) {
		it = set.insert(it, i);
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));

	set.clear();

	
	// OrderedSetup random integers
	std::srand(std::time(nullptr));
	std::vector<int> integers;
	for (int i = 0; i < N; ++i) {
		integers.emplace_back(i);
	}
	std::shuffle(integers.begin(), integers.end(), gen);

	// Test random integer insertion
	for (auto integer : integers) {
		auto ret = set.insert(integer);
		ASSERT_TRUE(ret.second);
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));

	set.clear();
	
	// Test random hint insertion
	it = set.begin();
	for (auto integer : integers) {
		set.insert(it, integer);
		it = set.begin() + std::rand() % set.size();
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));

	set.clear();
	
	// Duplicate value setup
	auto copy = integers;
	std::copy(copy.begin(), copy.end(), std::back_inserter(integers));
	std::shuffle(integers.begin(), integers.end(), gen);

	// Test random insertion with duplicates
	for (auto integer : integers) {
		set.insert(integer);
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_TRUE(is_unique(set.begin(), set.end()));
	
	set.clear();

	// Test random hinted insertion with duplicates
	it = set.begin();
	for (auto integer : integers) {
		set.insert(it, integer);
		it = set.begin() + std::rand() % set.size();
	}
	ASSERT_EQ(N, set.size());
	ASSERT_TRUE(std::is_sorted(set.begin(), set.end()));
	ASSERT_TRUE(is_unique(set.begin(), set.end()));
}

TEST(OrderedSetTests, ErasureTests) {
	set_type set;
	std::vector<int> integers;
	for (int i = 0; i < N; ++i) {
		integers.emplace_back(i);
		set.insert(set.end(), i);
	}
	set.insert(integers.begin(), integers.end());
	for (auto integer : integers) {
		ASSERT_EQ(1, set.erase(integer));
	}
	ASSERT_TRUE(set.empty());
	std::shuffle(integers.begin(), integers.end(), gen);
	set.insert(integers.begin(), integers.end());
	for (auto integer : integers) {
		auto it = set.find(integer);
		set.erase(it);
		ASSERT_FALSE(set.contains(integer));
	}
}

TEST(OrderedSetTests, LookupTests) {
	std::vector<int> integers;
	set_type set;
	for (int i = 1; i <= N; ++i) {
		integers.emplace_back(i);
		set.insert(set.end(), i);
	}
	std::shuffle(integers.begin(), integers.end(), gen);
	for (auto integer : integers) {
		ASSERT_FALSE(set.contains(-integer));
		ASSERT_EQ(0, set.count(-integer));
		ASSERT_TRUE(set.contains(integer)); // Tests find which tests lower_bound
		ASSERT_EQ(1, set.count(integer)); // Tests equal_range which tests lower_bound and upper_bound
	}
}

TEST(OrderedSetTests, LexicographicalTests) {
	ASSERT_EQ(set_type({ 1, 2, 3, 4 }), set_type({ 1, 2, 3, 4 }));
	ASSERT_LE(set_type({ 1, 2, 3, 4 }), set_type({ 2, 3, 4, 5 }));
}