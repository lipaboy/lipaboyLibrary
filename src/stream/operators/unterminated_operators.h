#pragma once

#include "tools.h"

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
namespace operators_space {

	using std::vector;
	using std::pair;
	using std::ostream;
	using std::string;
	using std::function;

	using std::cout;
	using std::endl;

	//#define LOL_DEBUG_NOISY

	using lipaboy_lib::function_traits;
	

	//-------------------------------------------------------------------------------------//
	//--------------------------------Unterminated operations------------------------------//
	//-------------------------------------------------------------------------------------//


	template <class Predicate>
	struct filter : FunctorHolder<Predicate>, TReturnSameType
	{
		static constexpr OperatorMetaTypeEnum metaInfo = FILTER;
		static constexpr bool isTerminated = false;
	public:
		filter(Predicate functor) : FunctorHolder<Predicate>(functor) {}

		template <class TSubStream>
		void init(TSubStream& stream)
		{
			hasNext<TSubStream>(stream);
		}

		template <class TSubStream>
		auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
			auto currElem = std::move(stream.nextElem());
			// ! calling hasNext() of current StreamType ! in order to skip unfilter elems
			hasNext<TSubStream>(stream);
			return std::move(currElem);
		}

		template <class TSubStream>
		void incrementSlider(TSubStream& stream) { stream.incrementSlider(); }

		template <class TSubStream>
		auto currentElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
			return std::move(stream.currentElem());
		}

		template <class TSubStream>
		bool hasNext(TSubStream& stream) { 
			for (; stream.hasNext(); stream.incrementSlider())
				if (true == functor()(std::move(currentElem<TSubStream>(stream))))
					return true;
			return false;
		}
	};

	template <class Transform>
	struct map : FunctorHolder<Transform> {
	public:
		template <class T>
		using RetType = typename std::result_of<Transform(T)>::type;

		static constexpr OperatorMetaTypeEnum metaInfo = MAP;
		static constexpr bool isTerminated = false;
	public:
		map(Transform functor) : FunctorHolder<Transform>(functor) {}

		template <class TSubStream>
		void init(TSubStream& stream)
		{}

		template <class TSubStream>
		auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
			return std::move(functor()(stream.nextElem()));
		}

		template <class TSubStream>
		void incrementSlider(TSubStream& stream) { stream.incrementSlider(); }

		template <class TSubStream>
		auto currentElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
			return std::move(functor()(stream.currentElem()));
		}

		template <class TSubStream>
		bool hasNext(TSubStream& stream) { return stream.hasNext(); }
	};

	struct get : TReturnSameType 
	{
	public:
		using size_type = size_t;

		static constexpr OperatorMetaTypeEnum metaInfo = GET;
		static constexpr bool isTerminated = false;
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
		void incrementSlider(TSubStream& stream) { 
			--size_;
			stream.incrementSlider(); 
		}

		template <class TSubStream>
		bool hasNext(TSubStream& stream) { return size() > 0 && stream.hasNext(); }

		size_type size() const { return size_; }

		bool operator==(get const & other) const { return size() == other.size(); }
		bool operator!=(get const & other) const { return !(*this == other); }

	private:
		size_type size_;
	};

	struct skip : TReturnSameType {
	public:
		using size_type = size_t;

		static constexpr OperatorMetaTypeEnum metaInfo = SKIP;
		static constexpr bool isTerminated = false;
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
		void incrementSlider(TSubStream& stream) { stream.incrementSlider(); }

		template <class TSubStream>
		bool hasNext(TSubStream& stream) { return stream.hasNext(); }

		size_type count() const { return count_; }

	private:
		size_type count_;
	};


}	

}

}

