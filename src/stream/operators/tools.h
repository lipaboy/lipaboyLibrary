#pragma once

#include "extra_tools/extra_tools.h"

#include <functional>
#include <iterator>
#include <typeinfo>
#include <type_traits>

namespace lipaboy_lib {

namespace stream_space {

namespace operators_space {

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
	//		---The same in ungroup_by_bit class!
	// TODO: wrap the temporary values into shared_ptr (in ungroup_by_bit)
	// TODO: write MovableStream (maybe you need move_iterators??)



	using std::function;

	#define LOL_DEBUG_NOISY

	using lipaboy_lib::function_traits;
	using lipaboy_lib::WrapBySTDFunctionType;

	enum OperatorMetaTypeEnum {
		FILTER,
		MAP,
		REDUCE,
		GET,
		SKIP,
		PRINT_TO,
		GROUP_BY_VECTOR,
		SUM,
		TO_VECTOR,
		NTH,
		UNGROUP_BY_BIT
	};


	//---------------Special structs--------------//

	struct TReturnSameType {
		template <class T>
		using RetType = T;
	};
	template <class Functor>
	struct FunctorMetaType {
		using GetMetaType = Functor;
	};

	template <class Functor>
	struct FunctorHolderDirectly : FunctorMetaType<Functor> {
		using OperatorType = Functor;
		FunctorHolderDirectly(OperatorType func) : functor_(func) {}

		OperatorType functor() const { return functor_; }
	private:
		OperatorType functor_;
	};

	// Wrap almost all the functions by std::function (except lambda with auto arguments and etc.)
	template <class Functor>
	struct FunctorHolderWrapper : FunctorMetaType<WrapBySTDFunctionType<Functor> > {
		using OperatorType = WrapBySTDFunctionType<Functor>;
		FunctorHolderWrapper(OperatorType func) : functor_(func) {}

		OperatorType functor() const { return functor_; }
	private:
		OperatorType functor_;
	};

	template <class Functor>
	struct FunctorHolder
		//        : FunctorHolderWrapper<Functor>
		: FunctorHolderDirectly<Functor>
	{
		FunctorHolder(Functor func)
			//        : FunctorHolderWrapper<Functor>(func)
			: FunctorHolderDirectly<Functor>(func)
		{}
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

}