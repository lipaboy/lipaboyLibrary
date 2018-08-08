#pragma once

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

	// TODO: refactor it
namespace operations_space {

	using std::vector;
	using std::pair;
	using std::ostream;
	using std::string;
	using std::function;

	using std::cout;
	using std::endl;

	//#define LOL_DEBUG_NOISY

	using lipaboy_lib::function_traits;
	using lipaboy_lib::WrapBySTDFunctionType;

	enum FunctorMetaTypeEnum {
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
		template <class Arg>
		struct RetType {
			using type = Arg;
		};
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


	//-------------------------------------------------------------------------------------//
	//--------------------------------Unterminated operations------------------------------//
	//-------------------------------------------------------------------------------------//


	template <class Predicate>
	struct filter : FunctorHolder<Predicate>, TReturnSameType
	{
		filter(Predicate functor) : FunctorHolder<Predicate>(functor) {}
		static constexpr FunctorMetaTypeEnum metaInfo = FILTER;
	};

	template <class Transform>
	struct map : FunctorHolder<Transform> {
	public:
		template <class Arg>
		struct RetType {
			using type = typename std::result_of<Transform(Arg)>::type;
		};

	public:
		map(Transform functor) : FunctorHolder<Transform>(functor) {}
		static constexpr FunctorMetaTypeEnum metaInfo = MAP;

		template <class Arg>
		auto operator()(Arg&& arg) const
			-> typename RetType<Arg>::type
		{
			return FunctorHolder<Transform>::functor()(std::forward<Arg>(arg));
		}
	};

	struct get : TReturnSameType {
		using size_type = size_t;
		get(size_type border) : border_(border) {}
		static constexpr FunctorMetaTypeEnum metaInfo = GET;

		size_type border() const { return border_; }

		bool operator==(get const & other) const { return border_ == other.border_; }
		bool operator!=(get const & other) const { return !(*this == other); }
	private:
		size_type border_;
	};

	struct group_by_vector {
		using size_type = size_t;

		group_by_vector(size_type partSize) : partSize_(partSize) {
			if (partSize == 0)
				throw std::logic_error("Parameter of GroupType constructor must be positive");
		}
		static constexpr FunctorMetaTypeEnum metaInfo = GROUP_BY_VECTOR;

		template <class Arg>
		struct RetType {
			using type = vector<Arg>;
		};

		size_type partSize() const { return partSize_; }
	private:
		size_type partSize_;
	};

	struct skip : TReturnSameType {
		using size_type = size_t;

		skip(size_type index) : index_(index) {}
		static constexpr FunctorMetaTypeEnum metaInfo = SKIP;

		size_type index() const { return index_; }
	private:
		size_type index_;
	};

	struct ungroupByBit {
		using size_type = size_t;

		static constexpr FunctorMetaTypeEnum metaInfo = UNGROUP_BY_BIT;

		template <class Arg>
		struct RetType {
			using type = bool;
		};
	};

}	

}

}

