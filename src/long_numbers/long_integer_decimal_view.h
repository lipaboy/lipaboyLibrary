#ifndef LONG_INTEGER_DECIMAL_VIEW_H
#define LONG_INTEGER_DECIMAL_VIEW_H

#include <iterator>
#include <array>

namespace lipaboy_lib::long_numbers_space {

using std::array;

namespace extra2 {

template <class TWord, class TSign>
TSign sign(bool isNegative, TWord const & word);

}

template <size_t lengthOfIntegrals>     // count of integral type variables
class LongIntegerDecimal;

template <size_t lengthOfIntegrals>
class LongIntegerDecimalView {
public:
    using ViewingType = LongIntegerDecimal<lengthOfIntegrals>;
    using IntegralType = typename ViewingType::IntegralType;
    using ContainerType = typename ViewingType::ContainerType;
    using IteratorType = typename ContainerType::const_iterator;
    using TSigned = typename ViewingType::TSigned;
    using size_type = size_t;

public:
    LongIntegerDecimalView(ViewingType const & longInteger)
            : begin_(longInteger.cbegin()),
            end_(longInteger.cend()),
            sign_(longInteger.sign())
    {}
    LongIntegerDecimalView(IteratorType begin, IteratorType end, TSigned sign)
            : begin_(begin),
            end_(end),
            sign_(sign)
    {}

    ViewingType operator+(LongIntegerDecimalView const & other);

    IteratorType begin() const { return begin_; }
    IteratorType end() const { return end_; }

    IntegralType operator[](size_type index) const { return *std::next(begin_, index); }
    size_type length() const { return lengthOfIntegrals; }
    size_type viewLength() const { return static_cast<size_type>(std::distance(begin_, end_)); }

    TSigned sign() const { return sign_; }
    void setSign(TSigned const& newSign) { sign_ = newSign; }

private:
    IteratorType begin_;
    IteratorType end_;
    TSigned sign_;
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
    for (auto iter = begin(); iter != end(); iter++, otherIter++) {
        const TSignedResult doubleTemp = TSignedResult(
            this->sign() * TSigned(*iter)
            + other.sign() * TSigned(*otherIter)
            + sign * TSigned(remainder)
        );

        result = IntegralType(std::abs(doubleTemp) % ViewingType::integralModulus());
        remainder = IntegralType(std::abs(doubleTemp) / ViewingType::integralModulus());
        sign = extra2::sign<TSignedResult, TSigned>(doubleTemp < 0, doubleTemp);
    }
    result.setSign(sign);

    return result;
}


}

#endif // LONG_INTEGER_DECIMAL_VIEW_H
