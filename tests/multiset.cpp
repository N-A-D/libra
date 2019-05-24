#include <gtest/gtest.h>
#include "../include/libva/ordered_multiset.hpp"

struct pair_compare {
	bool operator()(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) const {
		return lhs.first < rhs.first;
	}
};

using multiset_type = va::ordered_multiset<std::pair<int, int>, pair_compare>;
