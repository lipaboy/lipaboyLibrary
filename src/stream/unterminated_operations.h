#pragma once

#include "extra_tools/extra_tools.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <typeinfo>
#include <ostream>
#include <type_traits>

#include <any>

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

	// TODO: put off "Special structs" into own file or into extra_tools.h"

	struct TReturnSameType {
		template <class Arg>
		using RetType = Arg;
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
		using RetType = typename std::result_of<Transform(Arg)>::type;

	public:
		map(Transform functor) : FunctorHolder<Transform>(functor) {}
		static constexpr FunctorMetaTypeEnum metaInfo = MAP;

		template <class Arg>
		auto operator()(Arg&& arg) const
			-> RetType<Arg>
		{
			return FunctorHolder<Transform>::functor()(std::forward<Arg>(arg));
		}
	};

	struct get : TReturnSameType 
	{
	public:
		using size_type = size_t;

		static constexpr FunctorMetaTypeEnum metaInfo = GET;
	public:
		get(size_type size) : size_(size) {}

		template <class TSubStream>
		void init(TSubStream& stream)
		{}

		template <class TSubStream>
		auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
			--size_;
			return std::move(stream.nextElem());
		}

		template <class TSubStream>
		auto currentElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
			return std::move(stream.currentElem());
		}

		template <class TSubStream>
		bool hasNext(TSubStream& stream) { return size() > 0 && stream.hasNext(); }

		size_type size() const { return size_; }

		bool operator==(get const & other) const { return size() == other.size(); }
		bool operator!=(get const & other) const { return !(*this == other); }

	private:
		size_type size_;
	};

	struct group_by_vector {
	public:
		using size_type = size_t;

		template <class Arg>
		using RetType = vector<Arg>;

		static constexpr FunctorMetaTypeEnum metaInfo = GROUP_BY_VECTOR;
	public:
		group_by_vector(size_type partSize) : partSize_(partSize) {
			if (partSize == 0)
				throw std::logic_error("Parameter of GroupType constructor must be positive");
		}

		template <class TSubStream>
		void init(TSubStream& stream)
		{
			using ReturnType = RetType<typename TSubStream::ResultValueType>;
			currentElem_ = std::any(ReturnType());
			pCurrentElem<TSubStream>()->reserve(partSize());

			for (size_type i = 0; i < partSize() && stream.hasNext(); i++)
				pCurrentElem<TSubStream>()->push_back(std::move(stream.nextElem()));
		}

		template <class TSubStream>
		auto nextElem(TSubStream& stream) 
			-> RetType<typename TSubStream::ResultValueType> 
		{
			using ReturnType = RetType<typename TSubStream::ResultValueType>;
			ReturnType part;
			part.reserve(partSize());

			for (size_type i = 0; i < partSize() && stream.hasNext(); i++)
				part.push_back(std::move(stream.nextElem()));

			// swapping

			auto temp = std::move(part);
			part = std::move(std::any_cast<ReturnType>(currentElem_));
			currentElem_ = std::move(temp);

			return std::move(part);
		}

		template <class TSubStream>
		auto currentElem(TSubStream&) -> RetType<typename TSubStream::ResultValueType> {
			return *(this->template pCurrentElem<TSubStream>());
		}

		template <class TSubStream>
        bool hasNext(TSubStream& stream) { 
			return !(this->template pCurrentElem<TSubStream>()->empty())
				|| stream.hasNext(); 
		}

		size_type partSize() const { return partSize_; }

	private:
		template <class TSubStream>
		auto pCurrentElem() -> RetType<typename TSubStream::ResultValueType>* {
			return std::any_cast<RetType<typename TSubStream::ResultValueType> >(&currentElem_);
		}
		
	private:
		size_type partSize_;
		std::any currentElem_;
	};

	struct skip : TReturnSameType {
	public:
		using size_type = size_t;

		static constexpr FunctorMetaTypeEnum metaInfo = SKIP;
	public:
		skip(size_type count) : count_(count) {}

		template <class TSubStream>
		void init(TSubStream& stream) {
			for (size_type i = 0; i < count() && stream.hasNext(); i++) {
				// TODO: replace it on incrementSlider()
				stream.nextElem();
			}
		}

		template <class TSubStream>
		auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
			return std::move(stream.nextElem());
		}

		template <class TSubStream>
		auto currentElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
			return std::move(stream.currentElem());
		}

		template <class TSubStream>
		bool hasNext(TSubStream& stream) { return stream.hasNext(); }

		size_type count() const { return count_; }

	private:
		size_type count_;
	};

	struct ungroup_by_bit {
		using size_type = size_t;

		static constexpr FunctorMetaTypeEnum metaInfo = UNGROUP_BY_BIT;

		template <class Arg>
		using RetType = bool;
	};

}	

}

}

