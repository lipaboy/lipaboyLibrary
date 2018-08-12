#pragma once

#include "unterminated_operations.h"

#include "extra_tools/extra_tools.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>

#include <typeinfo>
#include <ostream>
#include <type_traits>

#include <iostream>

namespace lipaboy_lib {

namespace stream_space {

namespace operations_space {

	using std::vector;
	using std::pair;
	using std::ostream;
	using std::string;
	using std::function;

	using std::cout;
	using std::endl;


	//---------------------------------------------------------------------------------------------------//
	//-----------------------------------Terminated operations-------------------------------------------//
	//---------------------------------------------------------------------------------------------------//

	namespace {

		template <class Accumulator>
		using GetSecondArgumentType = typename function_traits<
			lipaboy_lib::WrapBySTDFunctionType<Accumulator> >::template arg<1>::type;

		using FalseType =
			//    std::false_type;
			std::function<void(void)>;

		//

		template <class T>
		struct result_of_else {
		};
		template <class F, class Arg>
		struct result_of_else<F(Arg)> {
			using type = typename std::result_of<F(Arg)>::type;
		};
		template <class Arg>
		struct result_of_else<FalseType(Arg)>
			: std::false_type
		{};
		template <class T>
		using result_of_else_t = typename result_of_else<T>::type;

		//

		template <class F, class Accumulator>
		struct GetFirstArgumentType_ElseArg {
			using type = typename function_traits<lipaboy_lib::WrapBySTDFunctionType<F> >::template arg<0>::type;
		};

		template <class Accumulator>
		struct GetFirstArgumentType_ElseArg<FalseType, Accumulator> {
			using type = GetSecondArgumentType<Accumulator>;
		};

	}

	template <class Accumulator, class IdentityFn = FalseType >
	struct reduce : FunctorHolder<Accumulator>,
		FunctorHolder<IdentityFn>
	{
	public:
		template <class TResult, class Arg>
		using AccumRetType = typename std::result_of<Accumulator(TResult, Arg)>::type;
		using IdentityFnType = IdentityFn;
		using ArgType = typename GetFirstArgumentType_ElseArg<IdentityFnType, Accumulator>::type;
		//    template <class ArgType>
		using IdentityRetType = lipaboy_lib::enable_if_else_t<std::is_same<IdentityFn, FalseType >::value,
			ArgType, result_of_else_t<IdentityFnType(ArgType)> >;

	public:
		reduce(IdentityFn&& identity, Accumulator&& accum)
			: FunctorHolder<Accumulator>(accum),
			FunctorHolder<IdentityFn>(identity)
		{}
		reduce(Accumulator&& accum)
			: FunctorHolder<Accumulator>(accum),
			FunctorHolder<IdentityFn>(FalseType())
		{}
		static constexpr FunctorMetaTypeEnum metaInfo = REDUCE;

		template <class TResult_, class Arg_>
		AccumRetType<TResult_, Arg_> accum(TResult_&& result, Arg_&& arg) const {
			return FunctorHolder<Accumulator>::functor()(std::forward<TResult_>(result),
				std::forward<Arg_>(arg));
		}
		template <class Arg_>
		IdentityRetType identity(Arg_&& arg) const
		{
			if constexpr (std::is_same<IdentityFn, FalseType>::value)
				return std::forward<Arg_>(arg);
			else
				return FunctorHolder<IdentityFn>::functor()(std::forward<Arg_>(arg));
		}

		template <class Stream_>
		auto apply(Stream_ & obj) -> typename reduce<Accumulator, IdentityFn>::IdentityRetType
		{
			using RetType = typename reduce<Accumulator, IdentityFn>::IdentityRetType;
			obj.init();
			if (obj.hasNext()) {
				auto result = identity(obj.nextElem());
				for (; obj.hasNext(); )
					result = accum(result, obj.nextElem());
				return result;
			}
			return RetType();
		}

	};

	struct sum {
		static constexpr FunctorMetaTypeEnum metaInfo = SUM;

		template <class TStream>
		auto apply(TStream & stream) -> typename TStream::ResultValueType 
		{
			using TResult = typename TStream::ResultValueType;
			//stream.assertOnInfiniteStream<TStream>();
			stream.init();
			auto result = (stream.hasNext()) ? stream.nextElem() : TResult();
			for (; stream.hasNext();)
				result += stream.nextElem();
			return std::move(result);
		}
	};

	struct print_to {
	public:
		print_to(std::ostream& o, string delimiter = "") : ostreamObj_(o), delimiter_(delimiter) {}
		static constexpr FunctorMetaTypeEnum metaInfo = PRINT_TO;

		template <class Stream_>
		std::ostream& apply(Stream_ & obj) {
			for (obj.init(); obj.hasNext(); )
				ostream() << obj.nextElem() << printer.delimiter();
			return ostream();
		}

		std::ostream& ostream() { return ostreamObj_; }
		string const & delimiter() const { return delimiter_; }
	private:
		std::ostream& ostreamObj_;
		string delimiter_;
	};

	struct to_vector {
		template <class T>
		using ContainerType = std::vector<T>;
		static constexpr FunctorMetaTypeEnum metaInfo = TO_VECTOR;

		template <class Stream_>
		auto apply(Stream_ & obj) -> vector<typename Stream_::ResultValueType>
		{
			using ToVectorType = vector<typename Stream_::ResultValueType>;
			ToVectorType toVector;
			for (obj.init(); obj.hasNext(); )
				toVector.push_back(obj.nextElem());
			return std::move(toVector);
		}
	};

	struct nth {
		using size_type = size_t;

		nth(size_type count) : count_(count) {}
		static constexpr FunctorMetaTypeEnum metaInfo = NTH;

		template <class Stream_>
		auto apply(Stream_ & obj) -> typename Stream_::ResultValueType
		{
			obj.init();
			for (size_t i = 0; i < count() && obj.hasNext(); i++)
				obj.nextElem();
			if (!obj.hasNext())
				throw std::logic_error("Stream (nth operation) : index is out of range");
			return obj.nextElem();
		}

		size_type count() const { return count_; }
	private:
		size_type count_;
	};

	//-------------------------------------------------------------//
	//--------------------------Apply API--------------------------//
	//-------------------------------------------------------------//

	
	
	
	


}

}

}

