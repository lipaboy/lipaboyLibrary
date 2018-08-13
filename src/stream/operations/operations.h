#pragma once

#include "terminated_operations.h"
#include "unterminated_operations.h"
#include "group_by_vector.h"

namespace lipaboy_lib {

namespace stream_space {

	// PLAN FOR STREAM:
	//-----------------
	// TODO: Think about allocators (in Range when happen copying and creating own container)
	//       (maybe too partical case?)
	// TODO: test different lambdas (with const&T in return type, with T& in argument type)
	// TODO: make Noisy test for reduce operation
	// TODO: think about writing iterators for Stream 
	//		 - it strange because Stream is a lazy iterator. It must be initialized. 
	//		   But std::algorithms doesn't call .init() method.
	//		 - Interesting fact: all the std::algorithms are terminated operations.
	//		 Stream is like specific iterator (like boost::transform_iterator)
	// TODO: instead of using std::any into group_by_vector (too many dynamic allocations)
	//		 you can replace it on two classes: group_by_vector and group_by_vector_impl<T>
	//		 where you store type into template parameter. Client will use ungeneric group_by_vector
	//		 but Stream will substitute it on generic group_by_vector_impl<T>
	// TODO: wrap the temporary values into shared_ptr (in group_by_vector and etc.)
	// TODO: write MovableStream


	namespace shortening {

		template <class TStream, class TOperation>
		struct StreamTypeExtender {
			using type = typename std::remove_reference_t<TStream>::
				template ExtendedStreamType<std::remove_reference_t<TOperation> >;
		};

		/*template <class TIterator>
		struct StreamTypeExtender<void, TIterator> {
		using type = Stream<TIterator>;
		};*/

		template <class TStream, class TOperation>
		using StreamTypeExtender_t = typename StreamTypeExtender<TStream, TOperation>::type;

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

