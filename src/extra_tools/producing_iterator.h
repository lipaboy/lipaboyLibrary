#pragma once

#include <functional>
#include <memory>
#include <type_traits>

namespace lipaboy_lib {

	//-----------------------------------------------------------------------//
	//---------------------PRODUCING ITERATOR BY FUNCTION--------------------//
	//-----------------------------------------------------------------------//

	// TODO: think about incrementing without saving value

	template <class T>
	class ProducingIterator {
	public:
		using GeneratorType = std::function<T(void)>;
		using value_type = T;
		using CurrentValueType = value_type;
		using CurrentValueTypePtr = std::shared_ptr<CurrentValueType>;
		using reference = T & ;
		using const_reference = const reference;
		using pointer = T * ;
		using const_pointer = const pointer;
		using iterator_category = std::input_iterator_tag;
		// It is type of measuring the distance between iterators (nothing else).
		// It is commented because ProducingIterator is not a real iterator. You can't count the distance
		// between iterators
        using difference_type = std::ptrdiff_t;

	public:
		ProducingIterator()
			: generator_(nullptr), pCurrentElem_(nullptr)
		{}
		ProducingIterator(GeneratorType gen)
			: generator_(gen), pCurrentElem_(std::make_shared<CurrentValueType>(std::move(gen())))
		{}
		ProducingIterator(ProducingIterator const & obj)
			: generator_(obj.generator_),
			pCurrentElem_(obj.pCurrentElem_)
		{}

		const_reference operator*() { return *pCurrentElem_; }
		const_pointer operator->() { return &(*pCurrentElem_); }

		// Note: not strong condition (maybe add counter to distinguishing the different iterators)
		bool operator== (ProducingIterator & other) {
            return generator_.template target<T(*)(void)>()
                    == other.generator_.template target<T(*)(void)>()
				&& pCurrentElem_ == other.pCurrentElem_
				&& *pCurrentElem_ == *(other.pCurrentElem_);
		}
		bool operator!= (ProducingIterator & other) { return !((*this) == other); }

		ProducingIterator operator++() {
			*pCurrentElem_ = std::move(generator_());
			return *this;
		}
		// Info: Return type is void because you cannot return previous iterator.
		//		 You cannot return previous iterator because pCurrentElem_ of different copies [iterators]
		//		 point to the same variable. If we don't have pointer to current elem then
		//		 we must have storage it directly (as a field). But copy constructor of iterator will be expensive.
		void operator++(int) {
			*pCurrentElem_ = std::move(generator_());
		}

	private:
		GeneratorType generator_;
		CurrentValueTypePtr pCurrentElem_;
	};


	// fast one

	template <class T>
	class ProducingIterator2 {
	public:
		using GeneratorType = std::function<T(void)>;
		using value_type = T;
		using CurrentValueType = value_type;
		using CurrentValueTypePtr = std::shared_ptr<CurrentValueType>;
		using reference = T & ;
		using const_reference = const reference;
		using pointer = T * ;
		using const_pointer = const pointer;
		using iterator_category = std::input_iterator_tag;
		// It is type of measuring the distance between iterators (nothing else).
		// It is commented because ProducingIterator is not a real iterator. You can't count the distance
		// between iterators
		using difference_type = std::ptrdiff_t;

	public:
		ProducingIterator2()
			: generator_(nullptr), elem_(CurrentValueType())
		{}
		ProducingIterator2(GeneratorType gen)
			: generator_(gen), elem_(gen())
		{}
		ProducingIterator2(ProducingIterator2 const & obj)
			: generator_(obj.generator_),
			elem_(obj.elem_)
		{}

		//const_reference
		value_type operator*() { return std::move(elem_); }
		const_pointer operator->() { return &elem_; }

		// Note: not strong condition (maybe add counter to distinguishing the different iterators)
		bool operator== (ProducingIterator2 & other) {
			return generator_.template target<T(*)(void)>()
				== other.generator_.template target<T(*)(void)>()
				&& elem_ == other.elem_;
		}
		bool operator!= (ProducingIterator2 & other) { return !((*this) == other); }

		ProducingIterator2 operator++() {
			elem_ = std::move(generator_());
			return *this;
		}
		// Info: Return type is void because you cannot return previous iterator.
		//		 You cannot return previous iterator because pCurrentElem_ of different copies [iterators]
		//		 point to the same variable. If we don't have pointer to current elem then
		//		 we must have storage it directly (as a field). But copy constructor of iterator will be expensive.
		void operator++(int) {
			elem_ = std::move(generator_());
		}

	private:
		GeneratorType generator_;
		CurrentValueType elem_;
	};
}


