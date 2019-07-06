#pragma once

#include <iterator>
#include <functional>
#include "../detail/extract_key.hpp"

namespace libra {
	namespace detail {
		template <class RndIt, class Key, class Compare, class ExtractKey>
		constexpr RndIt lower_bound(RndIt first, RndIt last, const Key& key, Compare comp, ExtractKey extract)
		{
			using diff_t = typename std::iterator_traits<RndIt>::difference_type;
			diff_t len = last - first;
			diff_t step;
			RndIt mid;
			while (len > 0) {
				mid = first;
				step = len / 2;
				mid += step;
				if (comp(extract(*mid), key)) {
					first = ++mid;
					len -= step + 1;
				}
				else
					len = step;
			}
			return first;
		}

		template <class RndIt, class Key, class Compare, class ExtractKey>
		constexpr RndIt upper_bound(RndIt first, RndIt last, const Key& key, Compare comp, ExtractKey extract)
		{
			using diff_t = typename std::iterator_traits<RndIt>::difference_type;
			diff_t len = last - first;
			diff_t step;
			RndIt mid;
			while (len > 0) {
				mid = first;
				step = len / 2;
				mid += step;
				if (!comp(key, extract(*mid))) {
					first = ++mid;
					len -= step + 1;
				}
				else
					len = step;
			}
			return first;
		}
	}

	template <class RndIt, class Key>
	constexpr RndIt lower_bound(RndIt first, RndIt last, const Key& key)
	{
		return libra::lower_bound(first, last, key, std::less<>{});
	}

	template <class RndIt, class Key, class Compare>
	constexpr RndIt lower_bound(RndIt first, RndIt last, const Key& key, Compare comp)
	{
		return detail::lower_bound(first, last, key, comp, detail::identity<Key>{});
	}

	template <class RndIt, class Key>
	constexpr RndIt upper_bound(RndIt first, RndIt last, const Key& key)
	{
		return libra::upper_bound(first, last, key, std::less<>{});
	}

	template <class RndIt, class Key, class Compare>
	constexpr RndIt upper_bound(RndIt first, RndIt last, const Key& key, Compare comp)
	{
		return detail::upper_bound(first, last, key, comp, detail::identity<Key>{});
	}

	template <class RndIt, class Key>
	constexpr bool binary_search(RndIt first, RndIt last, const Key& key)
	{
		return libra::binary_search(first, last, key, std::less<>{});
	}

	template <class RndIt, class Key, class Compare>
	constexpr bool binary_search(RndIt first, RndIt last, const Key& key, Compare comp)
	{
		first = libra::lower_bound(first, last, key, comp);
		return first != last && !(comp(key, *first));
	}

	template <class RndIt, class Key>
	constexpr std::pair<RndIt, RndIt> equal_range(RndIt first, RndIt last, const Key& key)
	{
		return libra::equal_range(first, last, key, std::less<>{});
	}

	template <class RndIt, class Key, class Compare>
	constexpr std::pair<RndIt, RndIt> equal_range(RndIt first, RndIt last, const Key& key, Compare comp)
	{
		return { libra::lower_bound(first, last, key, comp), libra::upper_bound(first, last, key, comp) };
	}

}