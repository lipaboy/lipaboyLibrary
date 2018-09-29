#pragma once

#include "tools.h"

#include <exception>

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			//---------------------------------------------------------------------------------------------------//
			//-----------------------------------Terminated operation-------------------------------------------//
			//---------------------------------------------------------------------------------------------------//
			

			struct nth {
				using size_type = size_t;

				template <class T>
				using RetType = T;

				nth(size_type count) : count_(count) {}
				static constexpr OperatorMetaTypeEnum metaInfo = NTH;
				static constexpr bool isTerminated = true;

				template <class Stream_>
				auto apply(Stream_ & obj) -> typename Stream_::ResultValueType
				{
					for (size_t i = 0; i < count() && obj.hasNext(); i++)
						obj.incrementSlider();
					if (!obj.hasNext())
						throw std::logic_error("Stream (nth operation) : index is out of range");
					return std::move(obj.nextElem());
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

