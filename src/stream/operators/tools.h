#pragma once

#include "extra_tools/extra_tools.h"

#include <functional>
#include <iterator>
#include <typeinfo>
#include <type_traits>

#include <optional>

namespace lipaboy_lib {

	namespace stream_space {

		namespace operators {

			// What's stream at all?
			// Answer: Stream is not iterator (because can't return current element). 
			//		   Single-pass. Save sequence of elements (periphrase).

			// PLAN FOR STREAM:
			//-----------------
			// TODO: Think about allocators (in Range when happen copying and creating own container)
			//       (maybe too partical case?)
			// TODO: test different lambdas (with const&T in return type, with T& in argument type)
			// TODO: make Noisy test for reduce operation
			// TODO: think about writing iterators for Stream 
			//			- Iterator must be storage temporary current values.
			//			- Interesting fact: all the std::algorithms are terminated operations.
			// TODO: write advises for them who will write operators for Stream, for example:
			//			- When you write operator and you have inner fields that must be initialized
			//			  by Stream's elem then you should do it into hasNext<>() because
			//			  likely it is first that called by all the terminated operations (or clients).
			//			  And furthermore you likely will call hasNext<>() into nextElem<>() and incrementSlider<>().
			//			  Or you can write private method init<>().
			//			- Note: if client call the hasNext<>() then he start using Stream. 
			//					It means that you can use all the API functions of Stream interface.



			using std::function;

			//#define LOL_DEBUG_NOISY

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
				UNGROUP_BY_BIT,

				DISTINCT,
				SPLIT,
				MAX
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
				using FunctorType = Functor;
				FunctorHolderDirectly(FunctorType func) : functor_(func) {}

				FunctorType functor() const { return functor_; }
				void setFunctor(FunctorType op) { functor_ = op; }
			private:
				FunctorType functor_;
			};

			// Wrap almost all the functions by std::function 
			// (except lambda with auto arguments and etc.)
			template <class Functor>
			struct FunctorHolderWrapper 
				: FunctorMetaType< WrapBySTDFunctionType<Functor> > 
			{
				using FunctorType = WrapBySTDFunctionType<Functor>;
				FunctorHolderWrapper(FunctorType func) : functor_(func) {}

				FunctorType functor() const { return functor_; }
				void setFunctor(FunctorType op) { functor_ = op; }
			private:
				FunctorType functor_;
			};

			template <>
			struct FunctorHolderWrapper< std::function<void(void)> >
			{
				FunctorHolderWrapper() {}
			};

			template <class Functor>
			struct FunctorHolderWrapperExcludeLambda 
				: FunctorMetaType< WrapBySTDFunctionExcludeLambdaType<Functor> > 
			{
				using FunctorType = WrapBySTDFunctionExcludeLambdaType<Functor>;
				FunctorHolderWrapperExcludeLambda(FunctorType func) : functor_(func) {}

				FunctorType functor() const { return functor_; }
				void setFunctor(FunctorType op) { functor_ = op; }
			private:
				FunctorType functor_;
			};

			template <class Functor>
			struct FunctorHolder
				   //     : FunctorHolderWrapper<Functor>
				//: FunctorHolderDirectly<Functor>
				: FunctorHolderWrapperExcludeLambda<Functor>
			{
				using Base = FunctorHolderWrapperExcludeLambda<Functor>;
				//       : FunctorHolderWrapper<Functor>(func)
					//: FunctorHolderDirectly<Functor>(func)
				FunctorHolder(typename Base::FunctorType func) 
					: Base(func)
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

			template <class TStream, class TOperator>
			struct TerminatedOperatorTypeApply {
                using type = std::remove_reference_t<TOperator>;
			};

			template <class TStream, class TOperator>
			using TerminatedOperatorTypeApply_t = 
				typename TerminatedOperatorTypeApply<TStream, TOperator>::type;
		}

	}

}
