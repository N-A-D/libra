#pragma once

namespace va {
	namespace detail {

		template <class T>
		struct identity {
			using type = T;
			const T& operator()(const T& t) const {
				return t;
			}
		};

		template <class Pair>
		struct select1st {
			using type = typename Pair::first_type;
			const type& operator()(const Pair& p) const {
				return p.first;
			}
		};

	}
}