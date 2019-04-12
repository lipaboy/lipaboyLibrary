#pragma once

#include <iterator>
#include <algorithm>
#include <fstream>
#include <vector>

namespace lipaboy_lib {

	// PLAN TODO
	// 1) Realize operator+= for working std::advance for this iterator

	template <class TNestedContainer>
	class NestedIterator {
	public:
		using NestedContainerType = TNestedContainer;
		using ExternalContainerType = typename NestedContainerType;
		using InnerContainerType = typename ExternalContainerType::value_type;
		using ElemType = typename InnerContainerType::value_type;
		using ExternalIterator = typename ExternalContainerType::iterator;
		using InternalIterator = typename InnerContainerType::iterator;

		using value_type = ElemType;
		using reference = value_type & ;
		using const_reference = const reference;
		using pointer = value_type * ;
		using const_pointer = const pointer;
		using iterator_category = std::random_access_iterator_tag;
		// Are you sure??
		using difference_type = int64_t;

	public:

	protected:
		NestedIterator(ExternalIterator externalIter, InternalIterator innerIter, ExternalIterator externalEnd)
			: externalIter_(externalIter), 
			internalIter_(innerIter),
			externalEnd_(externalEnd)
		{}

	public:

		static NestedIterator begin(NestedContainerType & container) {
			return NestedIterator(container.begin(), container.begin()->begin(), container.end());
		}

		static NestedIterator end(NestedContainerType & container) {
			auto iter = container.begin();
			return NestedIterator(container.end(), InternalIterator(), container.end());
		}

		bool operator==(NestedIterator const & other) {
			return externalIter_ == other.externalIter_
				&& internalIter_ == other.internalIter_;
		}
		bool operator!=(NestedIterator const & other) { return !((*this) == other); }

		NestedIterator operator++(int) {
			auto prevIter = (*this);

			internalIter_++;
			if (internalIter_ == externalIter_->end()) {
				externalIter_++;
				internalIter_ = !isEnd() ? externalIter_->begin() : InternalIterator();
			}

			return prevIter;
		}
		NestedIterator operator++() {
			(*this)++;
			return *this;
		}

		NestedIterator operator+=(difference_type diff) {
			this->advance(diff);
			return *this;
		}

		ElemType & operator*() {
			return *internalIter_;
		}

		// INFO: wouldn't be work with filestream's iterator (because single-pass)
		void advance(difference_type diff) {
			difference_type sign = (diff < 0) ? -1 : 1;
			typedef InternalIterator(InnerContainerType::*MemFunc)();
			MemFunc edgeFromFn,
				edgeToFn;

			if (diff < 0) {
				edgeFromFn = (&InnerContainerType::end);
				edgeToFn = (&InnerContainerType::begin);
			}
			else {
				edgeFromFn = (&InnerContainerType::begin);
				edgeToFn = (&InnerContainerType::end);
			}

			// BAD: infinite loops work bad in C++ because compiler can decide on it own when it can terminate the loop.
			//		You need to imagine situation where using NestedIterator with filestream is necessary.
			for (; ; ) {
				auto distance = std::distance(internalIter_, (*externalIter_.*edgeToFn)());
				if (diff > 0 && std::abs(diff) < std::abs(distance)
					|| diff < 0 && std::abs(diff) <= std::abs(distance)
					)
					break;
				diff -= distance;
				if (diff < 0)
					externalIter_--;
				else
					externalIter_++;
				if (isEnd())
					break;
				internalIter_ = (*externalIter_.*edgeFromFn)();
			}

			if (isEnd())	// it is correct and necessary condition
				internalIter_ = InternalIterator();
			else
				std::advance(internalIter_, diff);
		}

		bool isEnd() { return externalIter_ == externalEnd_; }

	private:
		ExternalIterator externalIter_;
		InternalIterator internalIter_;
		ExternalIterator externalEnd_;
	};

}