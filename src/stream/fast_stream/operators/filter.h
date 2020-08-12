#pragma once

#include "tools.h"

#include <memory>

namespace lipaboy_lib::fast_stream {

	namespace operators {

		using std::shared_ptr;
		using stream_space::operators::FunctorHolder;

		// INFO: you can remove intermediate type (filter) because you can deduce type of elems from Predicate's
		//		 argument.

		//-------------------------------------------------------------------------------------//
		//--------------------------------Unterminated operation------------------------------//
		//-------------------------------------------------------------------------------------//

		template <class Predicate>
		struct filter :
			public FunctorHolder<Predicate>,
			public stream_space::operators::TReturnSameType
		{
			static constexpr bool isTerminated = false;
		public:
			filter(Predicate functor) : stream_space::operators::FunctorHolder<Predicate>(functor) {}
		};

		template <class Predicate, class T>
		struct filter_impl :
			public stream_space::operators::FunctorHolder<Predicate>,
			public stream_space::operators::TReturnSameType
		{
			static constexpr bool isTerminated = false;
		public:
			filter_impl(filter<Predicate> obj)
				: FunctorHolder<Predicate>(obj.functor())
			{}

			// Opinion: difficult construction but without extra executions and computions

			template <class TSubStream>
			auto nextElem(TSubStream& stream) -> typename TSubStream::ResultValueType {
				// Info: I think you mustn't think about corrupting of branch predicator
				//		 because the gist of filtering is checking the conditions

				// ! calling hasNext() of current StreamType ! in order to skip unfilter elems
				auto curr = std::move(*elem_);
				incrementSlider<TSubStream>(stream);
				return std::move(curr);
			}

			template <class TSubStream>
			void incrementSlider(TSubStream& stream) {
				next<TSubStream>(stream);
				scamper<TSubStream>(stream);
			}

			// accepting such fact that client cannot call it before initialize() method.
			template <class TSubStream>
			bool hasNext(TSubStream& stream) {
				//return scamper<TSubStream>(stream);
				return (elem_ != nullptr);
			}

			template <class TSubStream>
			void initialize(TSubStream& stream) {
				stream.initialize();
				elem_ = std::make_shared<T>();
				next<TSubStream>(stream);
				scamper<TSubStream>(stream);
			}

		protected:
			template <class TSubStream>
			bool scamper(TSubStream& stream) {
				if (elem_ == nullptr)
					return false;
				while (false == FunctorHolder<Predicate>::functor()(*elem_)) {
					if (stream.hasNext())
						*elem_ = stream.nextElem();
					else {
						elem_ = nullptr;
						break;
					}
				}
				return (elem_ != nullptr);
			}

			template <class TSubStream>
			void next(TSubStream& stream) {
				//elem_ = stream.hasNext() ? std::move(stream.nextElem()) : T();
				if (stream.hasNext())
					*elem_ = stream.nextElem();
				else
					elem_ = nullptr;
			}

		private:
			shared_ptr<T> elem_ = nullptr;
		};

	}

	using operators::filter;
	using operators::filter_impl;

	template <class TStream, class Predicate>
	struct shortening::StreamTypeExtender<TStream, filter<Predicate> > {
		template <class T>
		using remref = std::remove_reference_t<T>;

		using type = typename remref<TStream>::template ExtendedStreamType<
			remref<filter_impl<Predicate, typename TStream::ResultValueType> > >;
	};

}

