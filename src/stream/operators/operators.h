#pragma once

#include "terminated_operators.h"
#include "unterminated_operators.h"
#include "group_by_vector.h"
#include "ungroup_by_bit.h"

namespace lipaboy_lib {

namespace stream_space {

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

