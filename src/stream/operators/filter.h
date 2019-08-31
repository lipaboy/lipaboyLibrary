#pragma once

#include "tools.h"

#include <memory>

namespace lipaboy_lib {

	namespace stream {

		namespace operators {

			using std::shared_ptr;

			// Contract rules : 
			//	1) filter_impl needs to store current element because
			//		when we call hasNext() it has pass the pack of elements
			//		to make sure the stream has next element indeed.

			// INFO: you can remove intermediate type (filter) because you can deduce type of elems from Predicate's
			//		 argument. BUT: you cannot pass lambda with auto parameter deducing.

			//-------------------------------------------------------------------------------------//
			//--------------------------------Unterminated operation------------------------------//
			//-------------------------------------------------------------------------------------//

			template <class Predicate>
			struct filter : public FunctorHolder<Predicate>, 
				public TReturnSameType
			{
				static constexpr OperatorMetaTypeEnum metaInfo = FILTER;
				static constexpr bool isTerminated = false;
			public:
				filter(Predicate functor) : FunctorHolder<Predicate>(functor) {}
			};

			template <class Predicate, class T>
			struct filter_impl : public FunctorHolder<Predicate>, 
				public TReturnSameType
			{
				static constexpr OperatorMetaTypeEnum metaInfo = FILTER;
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
					hasNext(stream);
					resetSaves();
					auto temp = std::move(*pCurrentElem_);
					if (stream.hasNext()) {
						*pCurrentElem_ = stream.nextElem();
						hasNext(stream);
					}
					else
						pCurrentElem_ = nullptr;
					return temp;
				}

				template <class TSubStream>
				void incrementSlider(TSubStream& stream) { 
					hasNext(stream);
					if (stream.hasNext()) {
						*pCurrentElem_ = std::move(stream.nextElem());
						resetSaves();
						hasNext(stream);
					}
				}

				template <class TSubStream>
				bool hasNext(TSubStream& stream) {
					if (isSavesActual_)
						return curr_;

					if (pCurrentElem_ == nullptr) {
						if (!stream.hasNext()) {
							saveResult(false);
							return false;
						}
						pCurrentElem_ = std::make_shared<T>(std::move(stream.nextElem()));
						resetSaves();
					}

					bool isHasNext = false;
					do {
						// Info: We don't have the right to std::move the content of pCurrentElem_
						if (true == FunctorHolder<Predicate>::functor()(*pCurrentElem_)) {
							saveResult(true);
							return true;
						}
						if (isHasNext = stream.hasNext()) {
							*pCurrentElem_ = std::move(stream.nextElem());
							resetSaves();
						}
					} while (isHasNext);

					saveResult(false);
					return false;
				}

			private:
				void saveResult(bool result) {
					isSavesActual_ = true;
					curr_ = result;
				}
				void resetSaves() {
					isSavesActual_ = false;
				}

			private:
				shared_ptr<T> pCurrentElem_ = nullptr;
				bool curr_;
				bool isSavesActual_ = false;
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

	//---------------------------------------------------------------------//
	//---------------------------------------------------------------------//
	//---------------------------------------------------------------------//

	namespace short_stream {

		namespace operators {

			using stream::operators::FunctorHolder;

			//-------------------------------------------------------------------------------------//
			//--------------------------------Unterminated operation------------------------------//
			//-------------------------------------------------------------------------------------//

			template <class Predicate>
			struct filter : public FunctorHolder<Predicate>,
				public TReturnSameType
			{
				static constexpr bool isTerminated = false;
			public:
				filter(Predicate obj)
					: FunctorHolder<Predicate>(obj)
				{}

				// Opinion: difficult construction but without extra executions and computions

				template <class TSubStream>
				auto next(TSubStream& stream) -> RetType<typename TSubStream::ResultValueType> {
					// Info: I think you mustn't think about corrupting of branch predicator
					//		 because the gist of filtering is checking the conditions

					// ! calling hasNext() of current StreamType ! in order to skip unfilter elems
					auto resOpt = stream.next();
					while (resOpt != std::nullopt && false == FunctorHolder<Predicate>::functor()(resOpt.value()))
						resOpt = stream.next();
					return resOpt;
				}

				/*template <class TSubStream>
				void incrementSlider(TSubStream& stream) {
					stream.incrementSlider();
				}
*/
			};

		}

		/*using operators::filter;
		using operators::filter_impl;

		template <class TStream, class Predicate>
		struct shortening::StreamTypeExtender<TStream, filter<Predicate> > {
			template <class T>
			using remref = std::remove_reference_t<T>;

			using type = typename remref<TStream>::template ExtendedStreamType<
				remref<filter_impl<Predicate> > >;
		};*/

	}

	//---------------------------------------------------------------------//
	//---------------------------------------------------------------------//
	//---------------------------------------------------------------------//

	namespace fast_stream {

		namespace operators {

			using std::shared_ptr;
            using stream::operators::FunctorHolder;

			// INFO: you can remove intermediate type (filter) because you can deduce type of elems from Predicate's
			//		 argument.

			//-------------------------------------------------------------------------------------//
			//--------------------------------Unterminated operation------------------------------//
			//-------------------------------------------------------------------------------------//

			template <class Predicate>
			struct filter : 
                public FunctorHolder<Predicate>,
				public stream::operators::TReturnSameType
			{
				static constexpr bool isTerminated = false;
			public:
                filter(Predicate functor) : stream::operators::FunctorHolder<Predicate>(functor) {}
			};

			template <class Predicate, class T>
			struct filter_impl : 
				public stream::operators::FunctorHolder<Predicate>,
				public stream::operators::TReturnSameType
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

}

