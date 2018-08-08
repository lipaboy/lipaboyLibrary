#pragma once

#include "stream_extended.h"
#include "extra_tools/extra_tools.h"

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>
#include <type_traits>

#include <typeinfo>

#include <iostream>

namespace lipaboy_lib {

namespace stream_space {

using std::vector;
using std::pair;
using std::string;

using std::cout;
using std::endl;
using lipaboy_lib::RelativeForward;


//--------------------------Stream Base (specialization class)----------------------//

template <class TIterator>
class Stream<TIterator> {
public:
    using T = typename std::iterator_traits<TIterator>::value_type;
    using ValueType = T;
    using size_type = size_t;
    using outside_iterator = TIterator;
    class Range;

    template <class Functor>
    using ExtendedStreamType = Stream<Functor, TIterator>;

    using ResultValueType = ValueType;

    template <typename, typename...> friend class Stream;

public:
    //----------------------Constructors----------------------//

    template <class OuterIterator>
    explicit
    Stream(OuterIterator begin, OuterIterator end) : range_(begin, end) {}
    explicit
    Stream(std::initializer_list<T> init) : range_(init) {}
    Stream(Stream const & obj) : range_(obj.range_)
    {
#ifdef LOL_DEBUG_NOISY
        cout << "   StreamBase copy-constructed" << endl;
#endif
    }
    Stream(Stream&& obj) noexcept : range_(std::move(obj.range_))
    {
#ifdef LOL_DEBUG_NOISY
        cout << "   StreamBase move-constructed" << endl;
#endif
    }
    Stream(typename Range::GeneratorTypePtr generator) : range_(generator) {}

    //----------------------Methods API-----------------------//

    // TODO: put off this methods into "global" function operators (for move-semantics of *this)

    auto operator| (get functor) -> ExtendedStreamType<get> {
        using ExtendedStream = ExtendedStreamType<get>;
        ExtendedStream newStream(functor, *this);
        newStream.action_ =
            [] (ExtendedStream * obj)
            {
                auto border = obj->operation().border();
                obj->range().makeFinite(border);
                obj->action_ = [] (ExtendedStream*) {};
            };
        return std::move(newStream);
    }
    auto operator| (group_by_vector functor) -> ExtendedStreamType<group_by_vector> {
        ExtendedStreamType<group_by_vector> newStream(functor, *this);
        return std::move(newStream);   // copy container (only once)
    }
    auto operator| (skip&& skipObj) -> ExtendedStreamType<skip> {
        using ExtendedStream = ExtendedStreamType<skip>;
        ExtendedStream newStream(skipObj, *this);
        newStream.action_ = [] (ExtendedStream* obj) {
            obj->range().moveBeginIter(obj->operation().index());
            obj->action_ = [] (ExtendedStream*) {};
        };
        return std::move(newStream);
    }

    auto operator| (ungroupByBit functor) -> ExtendedStreamType<ungroupByBit> {
        ExtendedStreamType<ungroupByBit> newStream(functor, *this);
        return std::move(newStream);
    }

    //-----------Terminated operations------------//

public:
    std::ostream& operator| (print_to&& printer) {
		assertOnInfiniteStream<Stream>();
        return apply(*this, std::move(printer));
    }

    template <class Accumulator, class IdentityFn>
    auto operator| (reduce<Accumulator, IdentityFn> const & reduceObj)
        -> typename reduce<Accumulator, IdentityFn>::IdentityRetType
    {
		assertOnInfiniteStream<Stream>();
        return apply(*this, reduceObj);
    }
    ResultValueType operator| (sum&&) {
		assertOnInfiniteStream<Stream>();
        init();
        if (hasNext()) {
            auto result = nextElem();
            for (; hasNext();)
                result += nextElem();
            return result;
        }
        return ResultValueType();
    }
	ResultValueType operator| (nth&& nthObj) {
		assertOnInfiniteStream<Stream>();
		return apply(*this, std::move(nthObj));
	}
    vector<ValueType> operator| (to_vector&& toVectorObj) {
		assertOnInfiniteStream<Stream>();
        return apply(*this, std::move(toVectorObj));
    }

    //------------------Additional methods---------------//

protected:
    Range & range() { return range_; }
    const Range & range() const { return range_; }
protected:
    static constexpr bool isNoGetTypeBefore() { return true; }
    static constexpr bool isNoGroupBefore() { return true; }
    static constexpr bool isGeneratorProducing() {
		return std::is_same_v<TIterator, ProducingIterator<ValueType> >;
	}
	static constexpr bool isInitilizerListCreation() {
		return std::is_same_v<TIterator, InitializerListIterator<ValueType> >;
	}
	static constexpr bool isOutsideIteratorsRefer() {
		return !isGeneratorProducing() && !isInitilizerListCreation();
	}

protected:
    // Info:
    // illusion of protected (it means that can be replace on private)
    // (because all the variadic templates are friends
    // from current Stream to first specialization) (it is not a real inheritance)

	template <class TStream_>
	inline static constexpr void assertOnInfiniteStream() {
		static_assert(!TStream_::isGeneratorProducing() || !TStream_::isNoGetTypeBefore(),
			"Stream error: attempt to work with infinite stream");
    }
protected:
    // TODO: think about this interface
    ValueType getElem(size_type index) const {
        return this->range().get(index);
    }

    //-----------------Slider API--------------//
public:
	void init() {}
    ResultValueType nextElem() { return std::move(range().nextElem()); }
    ValueType currentElem() { return std::move(range().currentElem()); }
    bool hasNext() { return range().hasNext(); }

    //-----------------Slider API Ends--------------//

public:
    bool operator==(Stream & other) { return equals(other); }
    bool operator!=(Stream & other) { return !((*this) == other); }
private:
    bool equals(Stream & other) { return range_.equals(other.range_); }

private:
    Range range_;
};



//--------------------------------------------------------------------------//
//------------------Extending stream by concating operations-----------------//
//--------------------------------------------------------------------------//


template <class TStream, class Transform>
auto operator| (TStream&& stream, map<Transform>&& functor)
    -> shortening::StreamTypeExtender<TStream, map<Transform> >
{
#ifdef LOL_DEBUG_NOISY
    cout << "---Add Map---" << endl;
#endif
    return shortening::StreamTypeExtender<TStream, map<Transform> >(functor, std::forward<TStream>(stream));
}


template <class TStream, class Predicate>
auto operator| (TStream&& stream, filter<Predicate> functor)
    -> shortening::StreamTypeExtender<TStream, filter<Predicate> >
{
    return shortening::StreamTypeExtender<TStream, filter<Predicate> >(functor, std::forward<TStream>(stream));
}


}

}