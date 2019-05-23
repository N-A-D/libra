#include <gtest/gtest.h>
#include "../include/libva/set.hpp"

struct pair_compare {
	bool operator()(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) const {
		return lhs.first < rhs.first;
	}
};

using multiset_type = va::multiset<std::pair<int, int>, pair_compare>;
