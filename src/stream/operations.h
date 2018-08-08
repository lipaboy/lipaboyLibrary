#pragma once

#include "terminated_operations.h"
#include "unterminated_operations.h"

namespace lipaboy_lib {

namespace stream_space {

	// PLAN FOR STREAM:
	//-----------------
	// TODO: remove duplication of code for terminated operators like
	//       it made for operator reduce
	//       This duplication lead to extra testing of code
	// TODO: think about condition of InfiniteStream when cause throwing an logic exception.
	//       Maybe put it into doPreliminaryOperations()
	//       And think about initSlider -> maybe move it into that one too?
	//		 doPreliminaryOperations to check if stream isGeneratorProducing and with NoGetTypeBefore
	// TODO: Think about allocators (in Range when happen copying and creating own container)
	//       (maybe too partical case?)
	// TODO: test different lambdas (with const&T in return type, with T& in argument type)
	// TODO: make Noisy test for reduce operation
	// TODO: make move-semantics for concating operations to stream
	// TODO: think about writing iterators for Stream
	//		 Stream is like specific iterator (like boost::transform_iterator)
	// TODO: test the allocating memory under tempOwner_ in ExtendedStream


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

