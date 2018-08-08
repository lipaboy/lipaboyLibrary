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
	using iterator_category = std::input_iterator_tag;
	using difference_type = std::ptrdiff_t;		// type of measuring the distance between iterators (nothing else)

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

	// Not strong condition
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

private:
	ContainerTypePtr pContainer_;
	Subiterator subiter_;
	// maybe I need to add the counter for distinguishing the iterators
};

}

}
