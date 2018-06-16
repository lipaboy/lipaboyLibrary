#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include <iterator>

#include <typeinfo>

#include <iostream>


#define IS_TEST_RUN229

#ifdef IS_TEST_RUN229
#include <gtest/gtest.h>

namespace stream_tests {
class StreamTest;
}

#endif

namespace stream_space {

namespace functors_space {

using std::vector;
using std::pair;
using std::ostream;
using std::string;

using namespace std::placeholders;

using std::cout;
using std::endl;

// PLAN FOR STREAM:
//-----------------
// TODO: realize slider interface (initSlider(), nextElem(), nextElem(), ...)
// TODO: think about filestream iterators because if you make one 'nth'
//       then you cannot return back to previous elements (symbols)
//       [single-pass iterators]

enum Info {
    GENERATOR,
    OUTSIDE_ITERATORS,
    INITIALIZING_LIST
};
struct IsGeneratorProducing {
    static constexpr Info info = GENERATOR;
};
struct IsOutsideIteratorsRefer {
    static constexpr Info info = OUTSIDE_ITERATORS;
};
struct IsInitializingListCreation {
    static constexpr Info info = INITIALIZING_LIST;
};

enum FunctorMetaTypeEnum {
    FILTER,
    MAP,
    REDUCE,
    GET,
    SKIP,
    PRINT_TO,
    GROUP,
    SUM,
    TO_VECTOR,
    NTH
};


//namespace special {

//---------------Special structs--------------//

struct TReturnSameType {
    template <class Arg>
    struct RetType {
        using type = Arg;
    };
};
template <class Functor>
struct FunctorMetaType {
    using GetMetaType = Functor;
};
// TODO: refactor, change names of meta functor and functor
template <class Functor>
struct FunctorHolder : FunctorMetaType<Functor> {
    FunctorHolder(Functor func) : functor_(func) {}
    Functor functor() const { return functor_; }
public:
    Functor functor_;
};

//}

//---------------Non-terminated operations-----------//

template <class Predicate>
struct filter : FunctorHolder<Predicate>, TReturnSameType {
    filter(Predicate functor)
        : FunctorHolder<Predicate>(functor) {}
    static constexpr FunctorMetaTypeEnum metaInfo = FILTER;
};
template <class Transform>
struct map : FunctorHolder<Transform> {
public:
    template <class Arg>
    struct RetType {
        using type = typename std::result_of<Transform(Arg)>::type;
    };

public:
    map(Transform functor) : FunctorHolder<Transform>(functor) {}
    static constexpr FunctorMetaTypeEnum metaInfo = MAP;

    template <class Arg>
    auto operator()(Arg arg) const
        -> typename RetType<Arg>::type
    {
        return FunctorHolder<Transform>::functor()(arg);
    }
};
struct get : TReturnSameType {
    using size_type = size_t;
    get(size_type border) : border_(border) {}
    static constexpr FunctorMetaTypeEnum metaInfo = GET;

    size_type border() const { return border_; }
private:
    size_type border_;
};
struct group {
    using size_type = size_t;

    group(size_type partSize) : partSize_(partSize) {
        if (partSize == 0)
            throw std::logic_error("Parameter of GroupType constructor must be positive");
    }
    static constexpr FunctorMetaTypeEnum metaInfo = GROUP;

    template <class Arg>
    struct RetType {
        using type = vector<Arg>;
    };

    size_type partSize() const { return partSize_; }
private:
    size_type partSize_;
};
struct skip : TReturnSameType {
    using size_type = size_t;

    skip(size_type index) : index_(index) {}
    static constexpr FunctorMetaTypeEnum metaInfo = SKIP;

    size_type index() const { return index_; }
private:
    size_type index_;
};

//---------------Terminated operations-----------//

template <class Accumulator, class IdentityFn = std::function<void(void)> >
struct reduce : FunctorHolder<Accumulator>,
                FunctorHolder<IdentityFn>
{
public:
    template <class TResult, class Arg>
    struct AccumRetType {
        using type = typename std::result_of<Accumulator(TResult, Arg)>::type;
    };
    using IdentityFnType = IdentityFn;
    template <class Arg>
    struct IdentityRetType {
        using type = typename std::result_of<IdentityFnType(Arg)>::type;
    };
public:
    reduce(IdentityFn&& identity, Accumulator&& accum)
        : FunctorHolder<Accumulator>(accum),
          FunctorHolder<IdentityFn>(identity)
    {}
    reduce(Accumulator&& accum)
        : FunctorHolder<Accumulator>(accum),
          FunctorHolder<IdentityFn>([] (void) -> void {})
    {}
    static constexpr FunctorMetaTypeEnum metaInfo = REDUCE;

    template <class TResult, class Arg>
    typename AccumRetType<TResult, Arg>::type accum(TResult&& result, Arg&& arg) const {
        return FunctorHolder<Accumulator>::functor()(std::forward<TResult>(result),
                                                     std::forward<Arg>(arg));
    }
    template <class Arg>
    decltype(auto) identity(Arg arg) const {
        if constexpr (std::is_same<IdentityFn, std::function<void(void)> >::value)
            return std::forward<Arg>(arg);
        else
            return FunctorHolder<IdentityFn>::functor()(std::forward<Arg>(arg));
    }
};
struct sum {
    static constexpr FunctorMetaTypeEnum metaInfo = SUM;
};
struct print_to {
public:
    print_to(std::ostream& o, string delimiter = "") : ostreamObj_(o), delimiter_(delimiter) {}
    static constexpr FunctorMetaTypeEnum metaInfo = PRINT_TO;

    std::ostream& ostream() { return ostreamObj_; }
    string const & delimiter() const { return delimiter_; }
private:
    std::ostream& ostreamObj_;
    string delimiter_;
};
struct to_vector {
    static constexpr FunctorMetaTypeEnum metaInfo = TO_VECTOR;
};
struct nth {
    using size_type = size_t;

    nth(size_type index) : index_(index) {}
    static constexpr FunctorMetaTypeEnum metaInfo = NTH;

    size_type index() const { return index_; }
private:
    size_type index_;
};


}

}

