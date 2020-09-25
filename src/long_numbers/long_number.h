#pragma once

#include <functional>
#include <iterator>
#include <numeric>
#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <cmath>
#include <tuple>

#include <string_view>
#include <charconv>

#include "extra_tools/extra_tools.h"
#include "intervals/cutoffborders.h"
#include "extra_tools/maths_tools.h"
#include "common_interfaces/comparable.h"

#include "long_integer_decimal_view.h"

namespace lipaboy_lib {

namespace long_numbers_space {

	// Motivation: I wanna check the theorem in Theory of Numbers 
	//			   (you know it or you can find it in pale red pocket book "Theory of Numbers in Cryptography")

	// PLAN
	// ----
	// TODO: write tests for serialization the number from string
	// THINK ABOUT: creating class LongIntegerViewer that doesn't able to change state
	//				of long number but can has different sign.
	//				It is related to constness property of such appearance. Half-const object property.
	// TODO: write requirements (see operator*)
	// TODO: you can write summarize of long numbers with variadic templates (expand all the integral numbers)
	// TODO: write LongInteger with std::vector (ExtendingInteger).
	// TODO: make project for doing benchmarks
    // THINK ABOUT: make the length() method by static (class behavior)
    // THINK ABOUT: move semantics through translating the type of container to class

using std::array;
using std::string;
using std::pair;

using lipaboy_lib::cutOffLeftBorder;
using lipaboy_lib::enable_if_else_t;
using lipaboy_lib::powDozen;
using lipaboy_lib::ComparatorExtender;

// Concept: it is simple long number, without any trivial optimizations like
//			checking if number is increasing or not (in order to making less computations)
//			and without move-semantics
// Hint: don't use std::vector for a while

////////////////////////////////////////////////////////////////////////////////////////

namespace extra {

	template <class TWord>
	inline constexpr size_t bitsCount() { return sizeof(TWord) * 8; }

    template <class TWord, class TSign>
    TSign sign(bool isNegative, TWord const & word) {
        return (isNegative * TSign(-1) + !isNegative * TSign(1))
            * (word != TWord(0));
    }

    //////////////////////////////////////////////////////////////////////////////////
    template <size_t val1, size_t val2>
    struct Max{
        static constexpr size_t value = (val1 < val2) ? val2 : val1;
    };
//    template <size_t val1, size_t val2>
//    using max = Max<val1, val2>::value;

    using LengthType = size_t;

    template <size_t len>
    LongIntegerDecimal<len> multiplyByKaracuba_(LongIntegerDecimalView<len> first, LongIntegerDecimalView<len> second);
}


using extra::LengthType;


// Requirements: 
// 1) TIntegral and TResult must be unsigned.

template <LengthType lengthOfIntegrals>     // count of integral type variables
class LongIntegerDecimal
        //: public ComparatorExtender<LongIntegerDecimal<lengthOfIntegrals> >
{
public:
	using TIntegral = std::uint32_t;
    using TIntegralResult = std::uint64_t;
	using TSigned = std::int32_t;
	using TSignedResult = std::int64_t;
    using LengthType = extra::LengthType;
    using size_type = size_t;

    using IntegralType =
        std::remove_reference_t<
            enable_if_else_t<2 * sizeof(TIntegral) == sizeof(TIntegralResult), TIntegral, void> >;
    using ResultIntegralType = std::remove_reference_t<TIntegralResult>;
	using ContainerType = array<IntegralType, lengthOfIntegrals>;
	using iterator = typename ContainerType::iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
	using const_iterator = typename ContainerType::const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	using reference = LongIntegerDecimal &;
	using const_reference = const LongIntegerDecimal&;
	using reference_integral = IntegralType & ;
	using const_reference_integral = const IntegralType&;

protected:
	LongIntegerDecimal(ContainerType const & number, bool minus)
		: number_(number), minus_(minus)
	{}

public:
	// Note: Non-initialized constructor: without filling array by zeroIntegral value.
	explicit
    LongIntegerDecimal() { checkTemplateParameters(); }
	LongIntegerDecimal(int small) : minus_(small < 0) {
		checkTemplateParameters();
		number_[0] = std::abs(small);
		std::fill(std::next(begin()), end(), TIntegral(0));
	}
	explicit
    LongIntegerDecimal(string const & numberDecimalStr);

	// TODO: calculate how much copy-constructor was called
    LongIntegerDecimal operator+(const_reference other) const { return (LongIntegerDecimal(*this) += other); }

    const_reference operator+=(const_reference other);

	// TODO: you can optimize it. When inverse operator is called then useless copy will be created.
	const_reference operator-=(const_reference other) { return (*this) += -other; }

	LongIntegerDecimal operator-(const_reference other) const {
		return (LongIntegerDecimal(*this) += -other);
	}

	LongIntegerDecimal operator-() const { return LongIntegerDecimal(number_, !minus_); }

    template <LengthType length2>
    auto operator*(LongIntegerDecimal<length2> const & other) const
        -> LongIntegerDecimal<extra::Max<lengthOfIntegrals, length2>::value >
    {
        using ResultType = LongIntegerDecimal<extra::Max<lengthOfIntegrals, length2>::value >;
        ResultType res(0);
        //		// This chapter has two parts
        //		// First part. Bisecting the result by two portions: main and overflow ones
        //		// Second part. Assigning the main portion into destination
        //		//				and saving the overflow one in order to take account at next multiplication.
        //		// Detail. There are two overflow portions that appear by two requirements:
        //		//		   1) Type can't storage more than it's capable.
        //		//		   2) Decimal type of this class.

        for (size_t i = 0; i < length(); i++)
        {
            IntegralType remainder = zeroIntegral();
            size_t j = 0;
            for (; i + j < res.length(); j++)
            {
                if (j >= other.length()) {
                    res[i + j] += remainder;
                    break;
                }

                const TIntegralResult doubleTemp = TIntegralResult((*this)[i]) * other[j] + remainder;
                // Detail #2
                res[i + j] += IntegralType(doubleTemp % integralModulus());
                remainder = IntegralType(doubleTemp / integralModulus());
            }
        }
        res.setSign(sign() * other.sign());

        return res;
    }

    template <LengthType length2>
    const_reference operator*=(LongIntegerDecimal<length2> const & other) {
        (*this) = (*this) * other;
        return *this;
    }

    LongIntegerDecimal operator/(const_reference other) const {
        return this->divide(other).first;
    }

    const_reference operator/=(const_reference other) {
        (*this) = (*this) / other;
        return *this;
    }

    LongIntegerDecimal operator%(const_reference other) const {
        return this->divide(other).second;
    }

    const_reference operator%=(const_reference other) {
        (*this) = (*this) % other;
        return *this;
    }

    auto divide(const_reference other) const -> pair<LongIntegerDecimal, LongIntegerDecimal>;

    LongIntegerDecimal multiplyByKaracuba(LongIntegerDecimal const & other) {
        auto res = extra::multiplyByKaracuba_<length()>(*this, other);
        res.setSign(sign() * other.sign());
        return res;
    }

	//-------------Converter---------------//

    string to_string() const;

	//------------Setters, Getters----------//

    static constexpr LengthType length() { return lengthOfIntegrals; }

        // O(N) algorithm because there is comparison for word
	    // sign() - #much-costs operation because it compares *this with zero number
    TSigned sign() const {
        return (minus_ * TSigned(-1) + !minus_ * TSigned(1))
            * (!isZero());
	}
	void setSign(TSigned sign) { minus_ = sign < 0; }
        // O(N)
    bool isNegative() const { return sign() < TSigned(0); }
        // O(N)
    bool isPositive() const { return sign() > TSigned(0); }
        // O(N)
    bool isZero() const {
        for (auto iter = cbegin(); iter != cend(); iter++)
            if (*iter != zeroIntegral())
                return false;
        return true;
    }

	    // Question: is it normal? Two methods have the same signature and live together?? 
	    //			 Maybe operator[] is exception of rules?
	const_reference_integral operator[] (size_t index) const { return number_[index]; }

	reference_integral operator[] (size_t index) { return number_[index]; }

    const_reference operator= (string const& numberStr) {
        this->assignString(numberStr);
        return *this;
    }

private:
    void assignString(string const& numberDecimalStr);

	//-------------------------Comparison---------------------------//

private:
    template <LengthType lengthFirst, LengthType lengthSecond>
    bool isLess(LongIntegerDecimal<lengthFirst> const & first, 
                LongIntegerDecimal<lengthSecond> const & second) const 
    {
        using FirstTypeIter = typename LongIntegerDecimal<lengthFirst>::iterator;
        using SecondTypeIter = typename LongIntegerDecimal<lengthSecond>::iterator;

        bool isNegative = first.isNegative();

        if (first.sign() * second.sign() < TSigned(0))		// #much-costs condition because see sign() -> (O(n))
            return isNegative;
        // true: first.isNegative() == second.isNegative() == isNegative

        bool isLessVar = true;
        bool isEqual = true;
        bool isResultDefined = false;
        auto iterF = first.crbegin();
        auto iterS = second.crbegin();
        auto checkHigherPartToZero =
            [&isLessVar, &isEqual, &isResultDefined] (auto& iter, const int lenHigh, const int lenLow)
                -> bool
        {
            bool partIsZero = true;
            // must cast all the vars to int because (I don't know)
            for (int i = 0; i < int(lenHigh) - int(lenLow); i++) {
                if (*iter != zeroIntegral()) {
                    partIsZero = false;
                    isEqual = false;
                    isResultDefined = true;
                    break;
                }
                iter++;
            }
            return isResultDefined ? partIsZero : isLessVar;
        };

        isLessVar = checkHigherPartToZero(iterF, lengthFirst, lengthSecond);
        if (!isResultDefined)
            isLessVar = !checkHigherPartToZero(iterS, lengthSecond, lengthFirst);

        if (!isResultDefined) {
            for (; iterF != first.crend() && iterS != second.crend(); iterF++, iterS++) {
                if (*iterF > *iterS) {
                    isLessVar = false;
                    isEqual = false;
                    break;
                }
                else if (*iterF < *iterS) {
                    isLessVar = true;
                    isEqual = false;
                    break;
                }
            }
        }
        return (!isEqual) && ((isNegative && !isLessVar) || (!isNegative && isLessVar));
    }

public:
    template <LengthType lengthOther>
	bool operator!= (LongIntegerDecimal<lengthOther> const & other) const {
        bool isEqual = true;
        auto iter = cbegin();
        auto iterO = other.cbegin();
        for (; iter != cend() && iterO != other.cend(); iter++, iterO++) {
            if (*iter != *iterO) {
                isEqual = false;
                break;
            }
        }
        if (isEqual) { // must have tests for this case
            for (; iter != cend(); iter++) {
                if (*iter != zeroIntegral()) {
                    isEqual = false;
                    break;
                }
            }
            for (; iterO != other.cend(); iterO++) {
                if (*iterO != zeroIntegral()) {
                    isEqual = false;
                    break;
                }
            }
        }
        return (sign() != other.sign() || !isEqual);
	}
    template <LengthType lengthOther>
    bool operator== (LongIntegerDecimal<lengthOther> const& other) const { return !(*this != other); }
    template <LengthType lengthOther>
    bool operator< (LongIntegerDecimal<lengthOther> const& other) const { return this->isLess(*this, other); }
   
    template <LengthType lengthOther>
    bool operator>= (LongIntegerDecimal<lengthOther> const& other) const { return !(*this < other); }
    template <LengthType lengthOther>
    bool operator> (LongIntegerDecimal<lengthOther> const& other) const { return this->isLess(other, *this); }
    template <LengthType lengthOther>
    bool operator<= (LongIntegerDecimal<lengthOther> const& other) const { return !(*this > other); }

public:
    // maximum count decimal digits that can be placed into IntegralType
    static constexpr IntegralType integralModulusDegree() {
		return static_cast<IntegralType>(std::floor(
			std::log(2) / std::log(10) * double(extra::bitsCount<IntegralType>()))); 
	}
    static constexpr IntegralType integralModulus() { return powDozen<IntegralType>(integralModulusDegree()); }
    static constexpr size_type maxDigitsCount() { return length() * integralModulusDegree(); }

private:
    static constexpr IntegralType zeroIntegral() { return IntegralType(0); }

public:
    // Return the remainder of division by 10
    IntegralType divideByDec();

    // TODO: Make private
public:
//private:
	iterator begin() { return number_.begin(); }
	iterator end() { return number_.end(); }
    reverse_iterator rbegin() { return number_.begin(); }
    reverse_iterator rend() { return number_.end(); }
	const_iterator cbegin() const { return number_.cbegin(); }
	const_iterator cend() const { return number_.cend(); }
    const_reverse_iterator crbegin() const { return number_.crbegin(); }
    const_reverse_iterator crend() const { return number_.crend(); }

private:
	void checkTemplateParameters() {
		static_assert(lengthOfIntegrals > 0, "Wrong length of LongInteger");
	}

private:
    // if index is increased then rank is increased
    array<IntegralType, lengthOfIntegrals> number_;
	bool minus_;

    friend class LongIntegerDecimalView<lengthOfIntegrals>;
};

//-------------------------------------------------------------//

template <LengthType length>
LongIntegerDecimal<length>::LongIntegerDecimal(string const & numberDecimalStr) : minus_(false) {
    checkTemplateParameters();
    if (numberDecimalStr.length() <= 0)
        LongIntegerDecimal();
    else
        assignString(numberDecimalStr);
}

template <LengthType length>
void LongIntegerDecimal<length>::assignString(string const& numberDecimalStr) {
    // TODO: add exception for zero length
    if (numberDecimalStr.length() > 0) {
        minus_ = false;
        std::string_view numStrView = numberDecimalStr;
        numStrView.remove_prefix(
            cutOffLeftBorder<int>(0, numStrView.find_first_not_of(" "))
        );
        if (numStrView[0] == '-') {
            numStrView.remove_prefix(1);
            minus_ = true; 
        }
        // round the number by integral modulus
        numStrView.remove_prefix(
            cutOffLeftBorder<int>(0, TSigned(numStrView.length()) - TSigned(maxDigitsCount()))
        );

        int last = int(numStrView.length());
        int first = cutOffLeftBorder<int>(last - integralModulusDegree(), 0);
        size_t i = 0;
        int subInt;

        for (; last - first > 0 && i < length(); i++) {
            auto sub = numStrView.substr(size_t(first), size_t(last) - size_t(first));
            std::from_chars(sub.data(), sub.data() + sub.size(), subInt);

            number_[i] = IntegralType(std::abs(subInt));

            last -= integralModulusDegree();
            first = cutOffLeftBorder<int>(first - integralModulusDegree(), 0);
        }
        std::fill(std::next(begin(), i), end(), zeroIntegral());
    }
}

//------------Arithmetic Operations-------------//

template <LengthType length>
auto LongIntegerDecimal<length>::operator+=(const_reference other)
    -> const_reference
{
    // Think_About: maybe std::partial_sum can be useful?

    IntegralType remainder = zeroIntegral();
    TSigned sign(1);
    for (size_t i = 0; i < length(); i++) {
        const TSignedResult doubleTemp = 
            TSignedResult(this->sign()) * (*this)[i]
            + TSignedResult(other.sign()) * other[i]
            + TSignedResult(sign) * remainder;

        (*this)[i] = IntegralType(std::abs(doubleTemp) % integralModulus());
        remainder = IntegralType(std::abs(doubleTemp) / integralModulus());
        sign = extra::sign<TSignedResult, TSigned>(doubleTemp < 0, doubleTemp);
    }
    this->setSign(sign);

    return *this;
}

template <LengthType length>
auto LongIntegerDecimal<length>::divide(const_reference other) const
    -> pair<LongIntegerDecimal, LongIntegerDecimal>
{
    // TODO: replace to OneDigitNumber
    const LongIntegerDecimal DEC(10);
    const LongIntegerDecimal ONE(1);
    const LongIntegerDecimal ZERO(0);

    LongIntegerDecimal divider(other);
    LongIntegerDecimal res(0);
    LongIntegerDecimal dividend(*this);

    LongIntegerDecimal modulus(1);
    // TODO: replace infinite loop to smth else
    while(true) {
        divider *= DEC;
        if (divider > dividend)
            break;
        modulus *= DEC;
    }

    divider.divideByDec();
    while(dividend >= divider || modulus != ONE) {
        while (dividend >= divider) {
            dividend -= divider;
            res += modulus;
        }

        while (modulus != ONE) {
            divider.divideByDec();
            modulus.divideByDec();
            if (divider <= dividend)
                break;
        }
    }
    // dividend - it is equal to remainder of division

    return std::make_pair(res, dividend);
}

template <LengthType length>
auto LongIntegerDecimal<length>::divideByDec() 
    -> IntegralType
{
    constexpr IntegralType DEC(10);
    IntegralType remainder(0);
    for (int i = int(length()) - 1; i >= 0; i--) {
        IntegralType newRemainder = (*this)[i] % DEC;
        (*this)[i] /= DEC;
        (*this)[i] += remainder * powDozen<IntegralType>(integralModulusDegree() - 1);
        remainder = newRemainder;
    }
    return remainder;
}

/////////////////////////////////////////////////////////////////////////////
//---------------------------------Karacuba--------------------------------//
/////////////////////////////////////////////////////////////////////////////

// Have a error

template <size_t len>
LongIntegerDecimal<len> extra::multiplyByKaracuba_(LongIntegerDecimalView<len> first, 
    LongIntegerDecimalView<len> second) {
    using LongNumberT = LongIntegerDecimal<len>;
    using LongNumberTView = LongIntegerDecimalView<len>;
    using IntegralType = typename LongNumberT::IntegralType;
    using DoubleType = typename LongNumberT::ResultIntegralType;
    const auto integralModulus = LongNumberT::integralModulus();
    const auto integralModulusDegree = LongNumberT::integralModulusDegree();

    LongIntegerDecimal<len> result(0);

    if (first.viewLength() == 1 && second.viewLength() == 1) {
        DoubleType mult = DoubleType(first[0]) * DoubleType(second[0]);
        result.setSign(1);
        result[0] = IntegralType(mult % DoubleType(integralModulus));
        if (len > 1)
            result[1] = IntegralType(mult / DoubleType(integralModulus));
    }
    else {
        LongNumberTView longerPart = (first.viewLength() >= second.viewLength()) ? first : second;
        LongNumberTView shorterPart = (first.viewLength() < second.viewLength()) ? first : second;
        auto halfSize = longerPart.viewLength() / 2;

        if (shorterPart.viewLength() > halfSize) {
            LongNumberTView minorLong(longerPart.begin(), std::next(longerPart.begin(), halfSize), 1);
            LongNumberTView minorShort(shorterPart.begin(),
                shorterPart.viewLength() > halfSize
                    ? std::next(shorterPart.begin(), halfSize)
                    : shorterPart.end(), 1);

            // Recursive descent
            LongNumberT minorMult = multiplyByKaracuba_(minorLong, minorShort);

            LongNumberTView majorLong(std::next(longerPart.begin(), halfSize), longerPart.end(), 1);
            LongNumberTView majorShort(std::next(shorterPart.begin(), halfSize), shorterPart.end(), 1);

            // Recursive descent
            LongNumberT majorMult = multiplyByKaracuba_(majorLong, majorShort);

            LongNumberT sumLong = majorLong + minorLong;
            LongNumberT sumShort = majorShort + minorShort;
            // Recursive descent
            LongNumberT multOfSums = multiplyByKaracuba_(
                        LongNumberTView(sumLong.begin(), std::next(sumLong.begin(), halfSize), 1),
                        LongNumberTView(sumShort.begin(), std::next(sumShort.begin(), halfSize), 1));

            LongNumberT differ = multOfSums - majorMult - minorMult;

            auto differShiftSize = integralModulusDegree * halfSize;
            result = minorMult + differ * powDozen<LongNumberT>(differShiftSize)
                    + majorMult * powDozen<LongNumberT>(2 * differShiftSize);
        }
        else {
            LongNumberTView minorLong(longerPart.begin(), std::next(longerPart.begin(), halfSize), 1);
            LongNumberTView minorShort(shorterPart.begin(),
                shorterPart.viewLength() > halfSize
                    ? std::next(shorterPart.begin(), halfSize)
                    : shorterPart.end(), 1);

            // Recursive descent
            LongNumberT minorMult = multiplyByKaracuba_(minorLong, minorShort);

            LongNumberTView majorLong(std::next(longerPart.begin(), halfSize), longerPart.end(), 1);

            LongNumberT sumLong = majorLong + minorLong;
            // Recursive descent
            LongNumberT multOfSums = multiplyByKaracuba_(
                        LongNumberTView(sumLong.begin(), std::next(sumLong.begin(), halfSize), 1),
                        minorShort);

            LongNumberT differ = multOfSums - minorMult;

            auto differShiftSize = integralModulusDegree  * halfSize;
            result = minorMult + differ * powDozen<LongNumberT>(differShiftSize);
        }

    }

    return result;
}

//----------------------------------------------------------------------------

template <size_t length>
string LongIntegerDecimal<length>::to_string() const {
    string res = (minus_) ? "-" : "";
    bool isFirstNonZeroMet = false;
    for (int i = int(length()) - 1; i >= 0; i--) {
        if (isFirstNonZeroMet || number_[i] != zeroIntegral()) {
            string part = std::to_string(number_[i]);
            // (integralModulusDegree() - part.size()) cannot be < 0 by the logic of class
            res += ((!isFirstNonZeroMet) ? ""
                : string(integralModulusDegree() - part.size(), '0'))
                + part;
            isFirstNonZeroMet = true;
        }
    }
    return (isFirstNonZeroMet) ? res : "0";
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

namespace interesting {

template <size_t length, size_t index>
struct Summarize {
	static void sum(
		LongIntegerDecimal<length> &		dest,
		LongIntegerDecimal<length> const &	src,
        typename LongIntegerDecimal<length>::IntegralType remainder
	)
	{
		using TIntegral = typename LongIntegerDecimal<length>::IntegralType;

		const typename LongIntegerDecimal<length>::TSignedResult
            doubleTemp = dest[index] + src[index] + remainder;
        dest[index] = TIntegral(std::abs(doubleTemp) % dest.integralModulus());
        TIntegral newRemainder = TIntegral(std::abs(doubleTemp) / dest.integralModulus());

        Summarize<length, index + 1>::sum(dest, src, newRemainder);
	}
};

template <size_t length>
struct Summarize<length, length>
{
	static void sum(
        LongIntegerDecimal<length> &		,
        LongIntegerDecimal<length> const &	,
        typename LongIntegerDecimal<length>::IntegralType
		)
	{}
};

}


}

}
