#pragma once

#include "tools.h"

#include <vector>
#include <type_traits>

#include <any>

namespace lipaboy_lib {

	namespace stream_space {

		// TODO: refactor it
		namespace operations_space {

			using std::vector;

			using std::cout;
			using std::endl;

			using lipaboy_lib::function_traits;

			struct group_by_vector {
			public:
				using size_type = size_t;

				template <class Arg>
				using RetType = vector<Arg>;

				static constexpr OperationMetaTypeEnum metaInfo = GROUP_BY_VECTOR;
				static constexpr bool isTerminated = false;
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

			


		}

	}

}

