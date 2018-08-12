#pragma once

#include <type_traits>
#include <functional>
#include <memory>

namespace lipaboy_lib {

namespace stream_space {

template <class T>
class InitializerListIterator {
public:
	using value_type = T;
	using ContainerType = std::vector<value_type>;
	using ContainerTypePtr = std::shared_ptr<ContainerType>;
	using Subiterator = typename ContainerType::iterator;
	using reference = T & ;
	using const_reference = const reference;
	using pointer = T * ;
	using const_pointer = const pointer;
	// you can't derive the iterator_category because you need implementations of contract responsibilities of abstraction
	using iterator_category = //std::iterator_traits<Subiterator>::iterator_category;
		std::input_iterator_tag;
	// type of measuring the distance between iterators (nothing else)
	using difference_type = typename std::iterator_traits<Subiterator>::difference_type;		

private:
	InitializerListIterator(ContainerTypePtr pContainer, Subiterator subiter)
		: pContainer_(pContainer),
		subiter_(subiter)
	{}
public:
	InitializerListIterator()
		: pContainer_(nullptr), subiter_()
	{}
	InitializerListIterator(std::initializer_list<T> init)
		: pContainer_(std::make_shared<ContainerType>(init))
	{
		subiter_ = pContainer_->begin();
	}
	InitializerListIterator(InitializerListIterator const & obj)
		: pContainer_(obj.pContainer_),
		subiter_(obj.subiter_)
	{}

	const_reference operator*() { return *subiter_; }
	const_pointer operator->() { return &(*subiter_); }

	bool operator== (InitializerListIterator const & other) const {
		return pContainer_ == other.pContainer_
			&& subiter_ == other.subiter_;
	}
	bool operator!= (InitializerListIterator const & other) const { return !((*this) == other); }

	InitializerListIterator operator++() {
		++subiter_;
		return *this;
	}
	InitializerListIterator operator++(int) {
		InitializerListIterator prev = *this;
		++subiter_;
		return prev;
	}

	//------------Own API------------//

	InitializerListIterator endIter() { return InitializerListIterator(pContainer_, pContainer_->end()); }

private:
	ContainerTypePtr pContainer_;
	Subiterator subiter_;
	// maybe I need to add the counter for distinguishing the iterators
};

}

}
