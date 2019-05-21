#include <gtest/gtest.h>
#include "../include/libva/set.hpp"
#include "utility.hpp"

using set_type = va::set<int>;

TEST(SetTests, ConstructorTests) {
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
	ASSERT_TRUE(is_unique(s2.begin(), s2.end()));
	ASSERT_TRUE(std::is_sorted(s2.begin(), s2.end()));

	// Tests construction from initializer list
	std::initializer_list<int> list{ 10, 5, 9, 4, 8, 3, 7, 2, 1, 6, 0, 22, 11 };
	set_type s3(list);
	ASSERT_EQ(s3.size(), list.size());
	ASSERT_TRUE(is_unique(s3.begin(), s3.end()));
	ASSERT_TRUE(std::is_sorted(s3.begin(), s3.end()));

	// Test construction from external container with duplicates
	size_t size = integers.size();
	for (size_t i = 0; i < size; ++i) {
		integers.emplace_back(static_cast<int>(i));
	}
	std::shuffle(integers.begin(), integers.end(), gen);
	set_type s4(integers.begin(), integers.end());
	ASSERT_EQ(s4.size(), size);
	ASSERT_TRUE(is_unique(s4.begin(), s4.end()));
	ASSERT_TRUE(std::is_sorted(s4.begin(), s4.end()));

	// Test construction from initializer list with duplicates
	std::initializer_list<int> list2{ 10, 5, 9, 4, 8, 3, 7, 2, 1, 6, 0, 22, 11, 10, 5, 9, 4, 8, 3, 7, 2, 1, 6, 0, 22, 11 };
	set_type s5(list2);
	ASSERT_EQ(s5.size(), list.size());
	ASSERT_TRUE(is_unique(s5.begin(), s5.end()));
	ASSERT_TRUE(std::is_sorted(s5.begin(), s5.end()));

	// Test copy construction
	set_type copier(s2);
	ASSERT_EQ(copier.size(), s2.size());
	ASSERT_TRUE(is_unique(copier.begin(), copier.end()));
	ASSERT_TRUE(std::is_sorted(copier.begin(), copier.end()));

	// Test move construction
	set_type thief(std::move(copier));
	ASSERT_TRUE(copier.empty());
	ASSERT_TRUE(is_unique(thief.begin(), thief.end()));
	ASSERT_TRUE(std::is_sorted(thief.begin(), thief.end()));

}

TEST(SetTests, AssignmentTests) {
	std::vector<int> integers;
	for (int i = 0; i < N; ++i) {
		integers.emplace_back(i);
	}
	std::shuffle(integers.begin(), integers.end(), gen);

	// Test copy assignment
	set_type s1(integers.begin(), integers.end());
	set_type s2 = s1;
	ASSERT_EQ(s1.size(), s2.size());
	ASSERT_TRUE(is_unique(s2.begin(), s2.end()));
	ASSERT_TRUE(std::is_sorted(s2.begin(), s2.end()));

	// Test move assignment
	set_type s3 = std::move(s2);
	ASSERT_TRUE(s2.empty());
	ASSERT_EQ(s1.size(), s3.size());
	ASSERT_TRUE(is_unique(s3.begin(), s3.end()));
	ASSERT_TRUE(std::is_sorted(s3.begin(), s3.end()));

	// Test initializer_list assignment
	std::initializer_list<int> list{ 4,7,2,1,6,5,3 };
	s3 = list;
	ASSERT_EQ(list.size(), s3.size());
	ASSERT_TRUE(is_unique(s3.begin(), s3.end()));
	ASSERT_TRUE(std::is_sorted(s3.begin(), s3.end()));
}

TEST(SetTests, InsertionTest) {}
