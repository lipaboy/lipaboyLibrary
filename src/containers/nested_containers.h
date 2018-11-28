#pragma once

namespace lipaboy_lib {

	template <class TNestedContainer>
	class NestedIterator {
	public:
		using NestedContainerType = TNestedContainer;
		using ExternalContainerType = typename NestedContainerType;
		using InnerContainerType = typename ExternalContainerType::value_type;
		using ElemType = typename InnerContainerType::value_type;
		using ExternalIterator = typename ExternalContainerType::iterator;
		using InnerIterator = typename InnerContainerType::iterator;

	public:

	protected:
		NestedIterator(ExternalIterator externalIter, InnerIterator innerIter, ExternalIterator externalEnd)
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
			return NestedIterator(container.end(), InnerIterator(), container.end());
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
				internalIter_ = externalIter_ != externalEnd_ ? externalIter_->begin() : InnerIterator();
			}

			return prevIter;
		}
		NestedIterator operator++() {
			(*this)++;
			return *this;
		}

		ElemType & operator*() {
			return *internalIter_;
		}

	private:
		ExternalIterator externalIter_;
		InnerIterator internalIter_;
		ExternalIterator externalEnd_;
	};

}