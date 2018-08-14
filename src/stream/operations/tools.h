#pragma once

#include "extra_tools/extra_tools.h"

#include <functional>
#include <iterator>
#include <typeinfo>
#include <type_traits>

namespace lipaboy_lib {

namespace stream_space {

namespace operations_space {

	using std::function;

	//#define LOL_DEBUG_NOISY

	using lipaboy_lib::function_traits;
	using lipaboy_lib::WrapBySTDFunctionType;

	enum OperationMetaTypeEnum {
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

	// TODO: put off "Special structs" into own file or into extra_tools.h"

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
		using OperationType = Functor;
		FunctorHolderDirectly(OperationType func) : functor_(func) {}

		OperationType functor() const { return functor_; }
	private:
		OperationType functor_;
	};

	// Wrap almost all the functions by std::function (except lambda with auto arguments and etc.)
	template <class Functor>
	struct FunctorHolderWrapper : FunctorMetaType<WrapBySTDFunctionType<Functor> > {
		using OperationType = WrapBySTDFunctionType<Functor>;
		FunctorHolderWrapper(OperationType func) : functor_(func) {}

		OperationType functor() const { return functor_; }
	private:
		OperationType functor_;
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

}

}