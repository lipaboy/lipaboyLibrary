#pragma once

#include "extra_tools/extra_tools.h"

#include <functional>
#include <iterator>
#include <typeinfo>
#include <type_traits>

#include <optional>

namespace lipaboy_lib::fast_stream {

	namespace shortening {

		//---------------StreamTypeExtender---------------//

		template <class TStream, class TOperator>
		struct StreamTypeExtender {
			using type = typename std::remove_reference_t<TStream>::
				template ExtendedStreamType<std::remove_reference_t<TOperator> >;
		};

		/*template <class TIterator>
		struct StreamTypeExtender<void, TIterator> {
		using type = StreamBase<TIterator>;
		};*/

		template <class TStream, class TOperator>
		using StreamTypeExtender_t = typename StreamTypeExtender<TStream, TOperator>::type;

	}

}

