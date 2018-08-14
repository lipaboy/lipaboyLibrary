#pragma once

#include "tools.h"

#include <vector>
#include <type_traits>
#include <memory>

namespace lipaboy_lib {

namespace stream_space {

// TODO: refactor it
namespace operations_space {

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

		//static constexpr OperationMetaTypeEnum metaInfo = GROUP_BY_VECTOR;
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

		static constexpr OperationMetaTypeEnum metaInfo = GROUP_BY_VECTOR;
		static constexpr bool isTerminated = false;
	public:
		group_by_vector_impl(size_type partSize) : partSize_(partSize) {
			if (partSize == 0)
				throw std::logic_error("Parameter of GroupType constructor must be positive");
		}
		group_by_vector_impl(group_by_vector const & groupObj) : group_by_vector_impl(groupObj.part()) 
		{}

		template <class TSubStream>
		void init(TSubStream& stream)
		{
			pCurrentElem_ = std::make_shared<ReturnType>();
			pCurrentElem_->reserve(partSize());

			for (size_type i = 0; i < partSize() && stream.hasNext(); i++)
				pCurrentElem_->push_back(std::move(stream.nextElem()));
		}

		template <class TSubStream>
		auto nextElem(TSubStream& stream)
			-> ReturnType
		{
			ReturnType part;
			part.reserve(partSize());

			for (size_type i = 0; i < partSize() && stream.hasNext(); i++)
				part.push_back(std::move(stream.nextElem()));

			std::swap(*pCurrentElem_, part);
			return std::move(part);
		}

		template <class TSubStream>
		auto currentElem(TSubStream&) -> ReturnType const & {
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

}

}

