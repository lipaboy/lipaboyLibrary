#pragma once

#include "extra_tools/extra_tools.h"

#include <functional>
#include <iterator>
#include <typeinfo>
#include <type_traits>

#include <optional>

namespace lipaboy_lib::short_stream {

	namespace operators {

		struct TOptionalReturnType {
			template <class T>
			using RetType = std::optional<T>;
		};

		struct TReturnSameType {
			template <class T>
			using RetType = T;
		};

		struct TGetValueType {
			template <class T>
			using RetType = typename T::value_type;
		};

	}

	namespace shortening {

		//---------------StreamTypeExtender---------------//

		template <class TStream, class TOperator>
		struct StreamTypeExtender {
			using type = typename std::remove_reference_t<TStream>::
				template ExtendedStreamType<std::remove_reference_t<TOperator> >;
		};

		/*template <class TIterator>
		struct StreamTypeExtender<void, TIterator> {
		using type = Stream<TIterator>;
		};*/

		template <class TStream, class TOperator>
		using StreamTypeExtender_t = typename StreamTypeExtender<TStream, TOperator>::type;

	}

}

