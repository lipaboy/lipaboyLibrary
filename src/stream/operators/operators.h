#pragma once

#include "tools.h"

// non-terminated operations
#include "filter.h"
#include "group_by_vector.h"
#include "get.h"
#include "skip.h"
#include "ungroup_by_bit.h"
#include "map.h"
#include "distinct.h"
#include "split.h"

//	   terminated operations
#include "nth.h"
#include "print_to.h"
#include "reduce.h"
#include "sum.h"
#include "to_vector.h"

namespace lipaboy_lib {

	namespace stream {

		namespace experimental {

			// smth interesting

			template <class T, class RelativeTo>
			struct relative_const {
				using type = T;
			};

			template <class T, class RelativeTo>
			struct relative_const<T, const RelativeTo> {
				using type = const T;
			};

			template <class T, class RelativeTo>
			using relative_const_t = typename relative_const<T, RelativeTo>::type;

		}

	}

}

