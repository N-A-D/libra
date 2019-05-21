#pragma once

#include <random>
#include <vector>

const int N = 100;

std::mt19937 gen{ std::random_device{}() };

template <class RndIt>
bool is_unique(RndIt first, RndIt last) {
	if (first == last)
		return true;
	auto next = first + 1;
	while (next != last) {
		if (*first == *next)
			return false;
		first = next++;
	}
	return true;
}

template <class RndIt, class BinaryPred>
bool is_unique(RndIt first, RndIt last, BinaryPred p) {
	if (first == last)
		return true;
	auto next = first + 1;
	while (next != last) {
		if (p(*first, *next))
			return false;
		first = next++;
	}
	return true;
}