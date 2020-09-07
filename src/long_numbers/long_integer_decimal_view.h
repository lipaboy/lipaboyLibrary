#ifndef LONG_INTEGER_DECIMAL_VIEW_H
#define LONG_INTEGER_DECIMAL_VIEW_H

#include <iterator>
#include <array>

namespace lipaboy_lib::long_numbers_space {

using std::array;

template <size_t lengthOfIntegrals>     // count of integral type variables
class LongIntegerDecimal;

template <size_t lengthOfIntegrals>
class LongIntegerDecimalView {
public:
    using ViewingType = LongIntegerDecimal<lengthOfIntegrals>;
    using IntegralType = typename ViewingType::IntegralType;
    using ContainerType = typename ViewingType::ContainerType;
    using IteratorType = typename ContainerType::const_iterator;
    using size_type = size_t;

public:
    LongIntegerDecimalView(ViewingType const & longInteger)
            : begin_(longInteger.cbegin()),
            end_(longInteger.cend())
    {}
    LongIntegerDecimalView(IteratorType begin, IteratorType end)
            : begin_(begin),
            end_(end)
    {}

    ViewingType operator+(LongIntegerDecimalView const & other);

    IteratorType begin() const { return begin_; }
    IteratorType end() const { return end_; }

    IntegralType operator[](size_type index) const { return *std::next(begin_, index); }
    size_type length() const { return lengthOfIntegrals; }
    size_type viewLength() const { return static_cast<size_type>(std::distance(begin_, end_)); }

private:
    IteratorType begin_;
    IteratorType end_;
};

template <size_t len>
auto LongIntegerDecimalView<len>::operator+(LongIntegerDecimalView const & other)
    -> ViewingType
{
    using TSignedResult = typename ViewingType::TSignedResult;
    using TSigned = typename ViewingType::TSigned;
    IntegralType remainder = ViewingType::zeroIntegral();
    TSigned sign(1);

    ViewingType result;

    auto otherIter = other.begin();
    for (auto iter = begin(); iter != end(); iter++) {
        const TSignedResult doubleTemp = TSignedResult(
            TSigned(*iter)
            + TSigned(*otherIter)
            + TSigned(remainder)
        );

        result = IntegralType(std::abs(doubleTemp) % ViewingType::integralModulus());
        remainder = IntegralType(std::abs(doubleTemp) / ViewingType::integralModulus());
//        sign = extra::sign<TSignedResult, TSigned>(doubleTemp < 0, doubleTemp);
    }

    return result;
}


}

#endif // LONG_INTEGER_DECIMAL_VIEW_H
