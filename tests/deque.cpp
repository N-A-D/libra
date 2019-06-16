#include <gtest/gtest.h>
#include <vector>
#include <random>
#include <algorithm>
#include "detail/constants.hpp"
#include "../include/libva/deque.hpp"

TEST(DequeTests, ConstructorTests) {
	va::deque<int> d1;

	d1.assign(10, 10);

	d1.assign({ 1, 2, 3 });
}

TEST(DequeTests, AssignmentTests) {

}

TEST(DequeTests, ElementAccessTests) {

}

TEST(DequeTests, IteratorTests) {

}

TEST(DequeTests, CapacityTests) {

}

TEST(DequeTests, InsertionTests) {

}

TEST(DequeTests, ErasureTests) {

}

TEST(DequeTests, PushAndPopTests) {

}

TEST(DequeTests, ResizeTests) {

}

TEST(DequeTests, SwapTests) {

}