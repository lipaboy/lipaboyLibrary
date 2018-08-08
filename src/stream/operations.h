#pragma once

#include "terminated_operations.h"
#include "unterminated_operations.h"

namespace lipaboy_lib {

namespace stream_space {

//----------------------------------------------------------------//
//--------------------functor_space ends--------------------------//
//----------------------------------------------------------------//

namespace shortening {

	template <class TStream, class TOperation>
	using StreamTypeExtender = typename std::remove_reference_t<TStream>::
		template ExtendedStreamType<std::remove_reference_t<TOperation> >;


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

}

