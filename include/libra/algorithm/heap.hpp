#pragma once

#include <cassert>
#include <iterator>
#include <functional>

namespace libra {
	namespace detail {
		template <class RndIt> using rnd_acc_diff_t = typename std::iterator_traits<RndIt>::difference_type;

		template <class RndIt, class Compare>
		constexpr void sift_up_heap(RndIt data, rnd_acc_diff_t<RndIt> idx, Compare comp)
		{
			while (idx > 0 && comp(data[(idx - 1) / 2], data[idx])) {
				std::swap(data[(idx - 1) / 2], data[idx]);
				idx = (idx - 1) / 2;
			}
		}

		template <class RndIt, class Compare>
		constexpr void sift_down_heap(RndIt data, rnd_acc_diff_t<RndIt> parent, rnd_acc_diff_t<RndIt> len, Compare comp)
		{
			while (2 * parent <= len) {
				auto child = 2 * parent;
				if (child < len && comp(data[child - 1], data[child]))
					++child;
				if (comp(data[parent - 1], data[child - 1])) {
					std::swap(data[parent - 1], data[child - 1]);
					parent = child;
				}
				else
					break;
			}
		}
	}

	template <class RndIt>
	constexpr void make_heap(RndIt first, RndIt last)
	{
		libra::make_heap(first, last, std::less<>{});
	}

	template <class RndIt, class Compare>
	constexpr void make_heap(RndIt first, RndIt last, Compare comp)
	{
		assert(first <= last);
		auto len = last - first;
		auto mid = len / 2;
		RndIt data = first;
		for (auto i = mid; i != 0; --i)
			detail::sift_down_heap(first, i, len, comp);
	}

	template <class RndIt>
	constexpr void push_heap(RndIt first, RndIt last)
	{
		libra::push_heap(first, last, std::less<>{});
	}

	template <class RndIt, class Compare>
	constexpr void push_heap(RndIt first, RndIt last, Compare comp)
	{
		assert(first < last);
		RndIt data = first;
		auto len = last - first;
		detail::sift_up_heap(first, len - 1, comp);
	}

	template <class RndIt>
	constexpr void pop_heap(RndIt first, RndIt last)
	{
		libra::pop_heap(first, last, std::less<>{});
	}

	template <class RndIt, class Compare>
	constexpr void pop_heap(RndIt first, RndIt last, Compare comp)
	{
		assert(first < last);
		if (first == std::prev(last))
			return;
		std::iter_swap(first, std::prev(last));
		libra::make_heap(first, std::prev(last), comp);
	}
}