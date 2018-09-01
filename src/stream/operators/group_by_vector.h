#pragma once

#include "tools.h"

#include <vector>
#include <type_traits>
#include <memory>

namespace lipaboy_lib {

namespace stream_space {

// TODO: refactor it
namespace operators_space {

	using std::vector;
	using std::shared_ptr;

	using std::cout;
	using std::endl;

	using lipaboy_lib::function_traits;

	struct group_by_vector {
	public:
		using size_type = size_t;

		template <class T>
		using RetType = vector<T>;

		//static constexpr OperatorMetaTypeEnum metaInfo = GROUP_BY_VECTOR;
		static constexpr bool isTerminated = false;
	public:
		group_by_vector(size_type partSize) : partSize_(partSize) {
			if (partSize == 0)
				throw std::logic_error("Parameter of GroupType constructor must be positive");
		}

		size_type part() const { return partSize_; }

	private:
		size_type partSize_;
	};

	template <class T>
	struct group_by_vector_impl {
	public:
		using size_type = size_t;

		template <class Arg_>
		using RetType = vector<Arg_>;
		using ReturnType = RetType<T>;
		using ReturnTypePtr = shared_ptr<ReturnType>;
		using const_reference = const ReturnType &;

		static constexpr OperatorMetaTypeEnum metaInfo = GROUP_BY_VECTOR;
		static constexpr bool isTerminated = false;
	public:
		group_by_vector_impl(size_type partSize) : partSize_(partSize) {
			if (partSize == 0)
				throw std::logic_error("Parameter of GroupType constructor must be positive");
		}
		group_by_vector_impl(group_by_vector const & groupObj) 
			: group_by_vector_impl(groupObj.part()) 
		{}

		template <class TSubStream>
		void init(TSubStream& stream)
		{
			pCurrentElem_ = std::make_shared<ReturnType>(partSize());
			incrementSlider<TSubStream>(stream);
		}

		template <class TSubStream>
		auto nextElem(TSubStream& stream)
			-> ReturnType
		{
			ReturnType part(partSize());
			std::swap(part, *pCurrentElem_);
			incrementSlider<TSubStream>(stream);
			return std::move(part);
		}

		template <class TSubStream>
		void incrementSlider(TSubStream& stream) {
			size_type i = 0;
			for (; i < partSize() && stream.hasNext(); i++)
				(*pCurrentElem_)[i] = (std::move(stream.nextElem()));
			pCurrentElem_->resize(i);
		}

		template <class TSubStream>
		auto currentElem(TSubStream&) -> const_reference {
			return *pCurrentElem_;
		}

		template <class TSubStream>
		bool hasNext(TSubStream& stream) {
			return !(pCurrentElem_->empty())
				|| stream.hasNext();
		}

		size_type partSize() const { return partSize_; }

	private:
		size_type partSize_;
		ReturnTypePtr pCurrentElem_ = nullptr;
	};

}

using operators_space::group_by_vector;
using operators_space::group_by_vector_impl;

template <class TStream>
struct shortening::StreamTypeExtender<TStream, group_by_vector> {
	template <class T>
	using remref = std::remove_reference_t<T>;

	using type = typename remref<TStream>::template ExtendedStreamType<
		remref<group_by_vector_impl<typename TStream::ResultValueType> > >;
};

}

}

