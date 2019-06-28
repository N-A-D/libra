#include <gtest/gtest.h>
#include "../include/libva/ordered_multiset.hpp"
#include "detail/constants.hpp"
#include <random>
#include <vector>
#include <algorithm>
#include "detail/employee.hpp"

using pair_type = std::pair<int, int>;

struct first_compare {
	bool operator()(const pair_type& lhs, const pair_type& rhs) const {
		return lhs.first < rhs.first;
	}
};


struct secondary_compare {
	bool operator()(const pair_type& lhs, const pair_type& rhs) const {
		return lhs.second < rhs.second;
	}
};

using multiset_type = va::ordered_multiset<pair_type, first_compare>;

std::mt19937 gen{ std::random_device{}() };

TEST(OrderedMultisetTests, ConstructorTests) {
	multiset_type ms1;
	ASSERT_TRUE(ms1.empty());

	std::vector<pair_type> pairs(N);
	std::generate(pairs.begin(), pairs.end(), [n = 0]() mutable { return std::pair(0, n++); });

	multiset_type ms2(pairs.begin(), pairs.end());
	ASSERT_EQ(N, ms2.size());
	ASSERT_TRUE(std::is_sorted(ms2.begin(), ms2.end(), ms2.value_comp()));
	ASSERT_TRUE(std::is_sorted(ms2.begin(), ms2.end(), secondary_compare()));

	multiset_type ms3(ms2);
	ASSERT_EQ(N, ms3.size());
	ASSERT_TRUE(std::is_sorted(ms3.begin(), ms3.end(), ms3.value_comp()));
	ASSERT_TRUE(std::is_sorted(ms3.begin(), ms3.end(), secondary_compare()));

	multiset_type ms4(std::move(ms3));
	ASSERT_TRUE(ms3.empty());
	ASSERT_EQ(N, ms4.size());
	ASSERT_TRUE(std::is_sorted(ms4.begin(), ms4.end(), ms4.value_comp()));
	ASSERT_TRUE(std::is_sorted(ms4.begin(), ms4.end(), secondary_compare()));

	for (int i = 1; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			pairs.emplace_back(std::pair(i, j));
		}
	}
	std::shuffle(pairs.begin(), pairs.end(), gen);

	multiset_type ms5(pairs.begin(), pairs.end());
	ASSERT_EQ(N * N, ms5.size());
	ASSERT_TRUE(std::is_sorted(ms5.begin(), ms5.end(), ms5.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, ms5.count(std::pair(i, 0)));
	}

	multiset_type ms6(ms5);
	ASSERT_EQ(N * N, ms6.size());
	ASSERT_TRUE(std::is_sorted(ms6.begin(), ms6.end(), ms6.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, ms6.count(std::pair(i, 0)));
	}

	multiset_type ms7(std::move(ms6));
	ASSERT_TRUE(ms6.empty());
	ASSERT_EQ(N * N, ms7.size());
	ASSERT_TRUE(std::is_sorted(ms7.begin(), ms7.end(), ms7.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, ms7.count(std::pair(i, 0)));
	}
}

TEST(OrderedMultisetTests, AssignmentTests) {
	// Setup
	std::vector<pair_type> pairs(N);
	std::generate(pairs.begin(), pairs.end(), [n = 0]() mutable { return std::make_pair(0, n++); });

	multiset_type ms1(pairs.begin(), pairs.end());
	ASSERT_EQ(N, ms1.count(std::pair(0, 0)));
	ASSERT_TRUE(std::is_sorted(ms1.begin(), ms1.end(), ms1.value_comp()));
	ASSERT_TRUE(std::is_sorted(ms1.begin(), ms1.end(), secondary_compare()));

	// Test order of insertion

	// Test copy assignment
	multiset_type ms2 = ms1;
	ASSERT_TRUE(!ms1.empty() && ms1.size() == N);
	ASSERT_EQ(N, ms2.count(std::pair(0, 0)));
	ASSERT_TRUE(std::is_sorted(ms2.begin(), ms2.end(), ms2.value_comp()));
	ASSERT_TRUE(std::is_sorted(ms2.begin(), ms2.end(), secondary_compare()));

	// Test move assignment
	multiset_type ms3 = std::move(ms2);
	ASSERT_TRUE(ms2.empty());
	ASSERT_EQ(N, ms3.count(std::pair(0, 0)));
	ASSERT_TRUE(std::is_sorted(ms3.begin(), ms3.end(), ms3.value_comp()));
	ASSERT_TRUE(std::is_sorted(ms3.begin(), ms3.end(), secondary_compare()));

	// Test container sortedness
	for (int i = 1; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			pairs.emplace_back(std::pair(i, j));
		}
	}

	// Test copy assignment
	multiset_type ms4(pairs.begin(), pairs.end());

	multiset_type ms5 = ms4;
	ASSERT_EQ(N * N, ms5.size());
	ASSERT_TRUE(std::is_sorted(ms5.begin(), ms5.end(), ms5.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, ms5.count(std::pair(i, i)));
	}

	// test move assignment
	multiset_type ms6 = std::move(ms4);
	ASSERT_TRUE(ms4.empty());
	ASSERT_EQ(N * N, ms6.size());
	ASSERT_TRUE(std::is_sorted(ms6.begin(), ms6.end(), ms6.value_comp()));
	for (int i = 0; i < N; ++i) {
		ASSERT_EQ(N, ms6.count(std::pair(i, i)));
	}
}

TEST(OrderedMultisetTests, InsertionTests) {
	std::srand(std::time(nullptr));
	std::vector<pair_type> pairs(N);
	std::generate(pairs.begin(), pairs.end(), [n = 0]() mutable { return std::make_pair(0, n++); });

	multiset_type multiset;

	for (auto pair : pairs) {
		auto ret = multiset.insert(pair);
		ASSERT_EQ(pair.first, ret->first);
	}
	ASSERT_EQ(N, multiset.count(std::pair(0, 0)));
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), multiset.value_comp()));
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), secondary_compare()));

	multiset.clear();

	multiset.insert(pairs.begin(), pairs.end());
	ASSERT_EQ(N, multiset.count(std::pair(0, 0)));
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), multiset.value_comp()));
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), secondary_compare()));

	multiset.clear();

	auto it = multiset.begin();
	for (auto pair : pairs) {
		auto ret = multiset.insert(it, pair);
		ASSERT_EQ(pair.first, ret->first);
		it = multiset.begin() + std::rand() % multiset.size();
	}
	ASSERT_EQ(N, multiset.count(std::pair(0, 0)));
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), multiset.value_comp()));
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), secondary_compare()));

	multiset.clear();

	for (int i = 1; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			pairs.emplace_back(std::make_pair(i, j));
		}
	}
	std::shuffle(pairs.begin(), pairs.end(), gen);

	for (auto pair : pairs) {
		auto ret = multiset.insert(pair);
		ASSERT_EQ(pair.first, ret->first);
	}
	ASSERT_EQ(N * N, multiset.size());
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), multiset.value_comp()));
	for (int i = 0; i < N; ++i)
		ASSERT_EQ(N, multiset.count(std::pair(i, i)));

	multiset.clear();

	multiset.insert(pairs.begin(), pairs.end());
	ASSERT_EQ(N * N, multiset.size());
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), multiset.value_comp()));
	for (int i = 0; i < N; ++i)
		ASSERT_EQ(N, multiset.count(std::pair(i, i)));

	multiset.clear();

	it = multiset.begin(); 
	for (auto pair : pairs) {
		auto ret = multiset.insert(it, pair);
		ASSERT_EQ(pair.first, ret->first);
		it = multiset.begin() + std::rand() % multiset.size();
	}
	ASSERT_EQ(N * N, multiset.size());
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), multiset.value_comp()));
	for (int i = 0; i < N; ++i)
		ASSERT_EQ(N, multiset.count(std::pair(i, i)));
}

TEST(OrderedMultisetTests, ErasureTests) {
	std::vector<pair_type> pairs;
	for (int i = 1; i <= N; ++i) {
		for (int j = 1; j <= N; ++j) {
			pairs.emplace_back(std::pair(i, j));
		}
	}
	std::shuffle(pairs.begin(), pairs.end(), gen);

	multiset_type multiset(pairs.begin(), pairs.end());
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), multiset.value_comp()));

	for (int i = 1; i <= N; ++i) {
		ASSERT_EQ(N, multiset.erase(std::pair(i, i)));
	}

	ASSERT_TRUE(multiset.empty());
	multiset.insert(pairs.begin(), pairs.end());
	for (int i = 1; i <= N; ++i) {
		int count = 0;
		while (multiset.contains(std::pair(i, i))) {
			auto it = multiset.find(std::pair(i, i));
			multiset.erase(it);
			++count;
		}
		ASSERT_EQ(N, count);
	}
}

TEST(OrderedMultisetTests, LookupTests) {
	std::vector<pair_type> pairs;
	for (int i = 1; i <= N; ++i) {
		for (int j = 1; j <= N; ++j) {
			pairs.emplace_back(std::pair(i, j));
		}
	}
	std::shuffle(pairs.begin(), pairs.end(), gen);

	multiset_type multiset(pairs.begin(), pairs.end());
	ASSERT_TRUE(std::is_sorted(multiset.begin(), multiset.end(), multiset.value_comp()));

	for (int i = 1; i <= N; ++i) {
		ASSERT_FALSE(multiset.contains(std::pair(-i, i)));
		ASSERT_EQ(0, multiset.count(std::pair(-i, i)));
		ASSERT_TRUE(multiset.contains(std::pair(i, i)));
		ASSERT_EQ(N, multiset.count(std::pair(i, i)));
	}
}

TEST(OrderedMultisetTests, LexicographicalTests) {
	ASSERT_EQ(multiset_type({ {0, 0}, {1, 1}, {2, 2} }), multiset_type({ {0, 0}, {1, 1}, {2, 2} }));
	ASSERT_LE(multiset_type({ {0, 0}, {1, 1}, {2, 2} }), multiset_type({ {1, 2}, {2, 5} }));
	ASSERT_GE(multiset_type({ {0, 0}, {1, 1}, {2, 2} }), multiset_type({ {0, 0}, {1, 1} }));
}

TEST(OrderedMultisetTests, SwapTests) {
	multiset_type mm1({ {0, 0}, {1, 1}, {2, 2} });
	multiset_type mm2({ {0, 0}, {1, 1}, {2, 2} });
	mm1.swap(mm2);
	ASSERT_EQ(mm2, multiset_type({ {0, 0}, {1, 1}, {2, 2} }));
	ASSERT_EQ(mm1, multiset_type({ {0, 0}, {1, 1}, {2, 2} }));
}

TEST(OrderedMultisetTests, DuplicateValueTransparentCompareTests) {
	using set_t = va::ordered_multiset<Employee, EmployeeCompare>;

	std::vector<int> ids;
	std::generate_n(std::back_inserter(ids), N, [n = 0]() mutable { return n++; });

	std::vector<Employee> emps;
	std::generate_n(std::back_inserter(emps), N, []() { return Employee{}; });

	auto copy(emps);
	std::copy(copy.begin(), copy.end(), std::back_inserter(emps));

	set_t set(emps.begin(), emps.end());

	for (auto emp : emps)
		ASSERT_EQ(set.count(emp), 2);

	for (auto id : ids)
		ASSERT_EQ(set.count(id), 2);
}