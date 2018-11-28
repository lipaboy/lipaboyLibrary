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

	private:
		ExternalIterator externalIter;
		InnerIterator internalIter;
	};

}