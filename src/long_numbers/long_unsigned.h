#pragma once

#include "long_number_base.h"

#include <algorithm>    // std::fill, std::next
#include <numeric>      // std::numeric_limits
#include <array>
#include <cstdint>      // uint64_t, ...
#include <cmath>        // std::min, std::log
#include <tuple>        // std::pair
#include <optional>

// building number from string
#include <string>
#include <string_view>
#include <charconv>
#include <ostream>
#include "intervals/cutoffborders.h"

#if (defined(WIN32) && defined(DEBUG_)) || (defined(__linux__) && !defined(NDEBUG))
#include <exception>    // check division by zero
#endif

#include "extra_tools/extra_tools.h"    // enable_if_else_t
#include "extra_tools/maths_tools.h"    // special::pow

namespace lipaboy_lib::long_numbers_space {

    // Motivation: I wanna check the theorem in Theory of Numbers
    //			   (you know it or you can find it in pale red pocket book "Theory of Numbers in Cryptography")

    // PLAN
    // ----
    // See LongIntegerDecimal
    // 1) Write new Algebra with another realization: instead of use method 'getNumber()'
    //      for wrapper class you need to cast the primary type to wrapper class.
    //      Like: (*this) + LongUnsigned(value)

    using std::array;
    using std::pair;

    using extra::LengthType;
    using lipaboy_lib::cutOffLeftBorder;

    // Concept: it is simple long number, without any trivial optimizations like
    //			checking if number is increasing or not (in order to making less computations)
    //			and without move-semantics

    ////////////////////////////////////////////////////////////////////////////////////////

    template <LengthType countOfIntegrals>     // count of integral type variables
    class LongUnsigned : 
        public LongNumberBase<uint32_t, uint64_t, countOfIntegrals>
    {
    public:
        using Sub = LongNumberBase<uint32_t, uint64_t, countOfIntegrals>;
        using IntegralType = typename Sub::IntegralType;
        using ResultIntegralType = typename Sub::ResultIntegralType;

        using ContainerType = array<IntegralType, countOfIntegrals>;
        using iterator = typename ContainerType::iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_iterator = typename ContainerType::const_iterator;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        using reference = LongUnsigned&;
        using const_reference = const LongUnsigned&;
        using reference_integral = IntegralType&;
        using const_reference_integral = const IntegralType&;

        using Sub::length;
        using Sub::zeroIntegral;
        using Sub::integralModulus;
        using Sub::integralModulusDegree;

    public:
        template <LengthType otherCountOfIntegrals> 
        friend class LongUnsigned;

    protected:
        LongUnsigned(ContainerType const& number)
            : number_(number)
        {}

    public:
        // Note: Non-initialized constructor: without filling array by zeroIntegral value.
        LongUnsigned() {}

        LongUnsigned(IntegralType small) {
            number_[0] = small;
            std::fill(std::next(begin()), end(), zeroIntegral());
        }

        explicit
            LongUnsigned(std::string_view numberStr, unsigned int base = 10);

        template <LengthType otherCount>
        explicit
            LongUnsigned(LongUnsigned<otherCount> const & other) 
        {
            auto minLen = (other.length() > length()) ? length() : other.length();
            std::copy_n(other.cbegin(), minLen, std::begin(number_));
            std::fill(std::next(begin(), minLen), end(), zeroIntegral());
        }

        //---------------Arithmetic operations------------------//

        template <LengthType first, LengthType second>
        using LongUnsignedResult = LongUnsigned< extra::Max<first, second>::value >;

        // TODO: calculate how much copy-constructor was called
        template <LengthType otherCount>
        auto operator+(LongUnsigned<otherCount> const& other) const
            -> LongUnsignedResult<countOfIntegrals, otherCount>
        {
            using ResultType = LongUnsignedResult<countOfIntegrals, otherCount>;
            return (ResultType(*this) += other);
        }

        template <LengthType otherCount>
        auto operator+=(LongUnsigned<otherCount> const & other)
            -> const_reference
        {
            constexpr auto MIN_LENGTH = std::min(countOfIntegrals, otherCount);
            IntegralType remainder = zeroIntegral();
            size_t i = 0;
            for (; i < MIN_LENGTH; i++) {
                const ResultIntegralType dualTemp =
                    ResultIntegralType((*this)[i])
                    + ResultIntegralType(other[i])
                    + ResultIntegralType(remainder);

                (*this)[i] = IntegralType(dualTemp & integralModulus());
                remainder = IntegralType(dualTemp >> integralModulusDegree());
            }
            if constexpr (length() > MIN_LENGTH) {
                // TODO: not optimal, because you needn't to go to the end of number. 
                // Only to the next of last traversal (next after min_length)
                for (; i < length(); i++) {
                    const ResultIntegralType dualTemp =
                        ResultIntegralType((*this)[i])
                        + ResultIntegralType(remainder);

                    (*this)[i] = IntegralType(dualTemp & integralModulus());
                    remainder = IntegralType(dualTemp >> integralModulusDegree());
                }
            }

            return *this;
        }

        // TODO: you can optimize it. When inverse operator is called then useless copy will be created.
        template <LengthType otherCount>
        LongUnsigned operator-(LongUnsigned<otherCount> const& other) const {
            return (LongUnsigned(*this) -= other);
        }

        template <LengthType otherCount>
        auto operator-=(LongUnsigned<otherCount> const& other)
            -> const_reference
        {
            constexpr auto MIN_LENGTH = std::min(countOfIntegrals, otherCount);
            IntegralType remainder = zeroIntegral();
            size_t i = 0;
            for (; i < MIN_LENGTH; i++) {
                const ResultIntegralType dualTemp =
                    ResultIntegralType((*this)[i])
                    - ResultIntegralType(other[i])
                    - ResultIntegralType(remainder);

                (*this)[i] = IntegralType(dualTemp & integralModulus());
                // "2*x - 1" Explanation: it works because if (*this)[i] - other[i] < 0 then
                //      dualTemp will be look like 111..111xx..xx as 64-bit number - 
                //      and the first bit will be 1 - it is out remainder.
                remainder = IntegralType(dualTemp >> (2 * integralModulusDegree() - 1));
            }
            if constexpr (length() > MIN_LENGTH) {
                for (; i < length(); i++) {
                    const ResultIntegralType dualTemp =
                        ResultIntegralType((*this)[i])
                        - ResultIntegralType(remainder);

                    (*this)[i] = IntegralType(dualTemp & integralModulus());
                    remainder = IntegralType(dualTemp >> (2 * integralModulusDegree() - 1));
                }
            }

            return *this;
        }

        template <LengthType otherCount>
        auto operator*(LongUnsigned<otherCount> const& other) const
            -> LongUnsignedResult<countOfIntegrals, otherCount>
        {
            using ResultType = LongUnsignedResult<countOfIntegrals, otherCount>;
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

                    const ResultIntegralType dualTemp = 
                        ResultIntegralType((*this)[i]) * other[j] + remainder;
                    // Detail #2
                    res[i + j] += IntegralType(dualTemp & integralModulus());
                    remainder = IntegralType(dualTemp >> integralModulusDegree());
                }
            }

            return res;
        }

        template <LengthType otherCount>
        const_reference operator*=(LongUnsigned<otherCount> const& other) {
            (*this) = (*this) * other;
            return *this;
        }

        LongUnsigned operator/(const_reference other) const {
            return this->divide(other).first;
        }

        const_reference operator/=(const_reference other) {
            (*this) = (*this) / other;
            return *this;
        }

        LongUnsigned operator%(const_reference other) const {
            return this->divide(other).second;
        }

        const_reference operator%=(const_reference other) {
            (*this) = (*this) % other;
            return *this;
        }

        template <LengthType otherCount>
        auto divide(LongUnsigned<otherCount> const & other) const -> pair<LongUnsigned, LongUnsigned>
        {
            const LongUnsigned<1> DEC(10);
            const LongUnsigned<1> ONE(1);
            const LongUnsigned<1> ZERO(0);

            #if (defined(WIN32) && defined(DEBUG_)) || (defined(__linux__) && !defined(NDEBUG))
                if (other == ZERO) {
                    throw std::runtime_error("Runtime Error (LongUnsigned): division by zero");
                }
            #endif

            LongUnsigned quotient(0);
            LongUnsigned dividend(*this);
            LongUnsigned divider(other);
            LongUnsigned modulus(1);

            int dividendMajorBit = int(dividend.majorBitPosition().value_or(0));
            int dividerMajorBit = int(divider.majorBitPosition().value_or(0));

            int diff = dividendMajorBit - dividerMajorBit;
            if (diff > 0) {
                divider.shiftLeft(diff);
                modulus.shiftLeft(diff);
                if (divider > dividend) {
                    divider.shiftRight(1);
                    modulus.shiftRight(1);
                }
            }

            while (dividend >= divider || modulus != ONE) {
                while (dividend >= divider) {
                    dividend -= divider;
                    quotient += modulus;
                }

                while (modulus != ONE) {
                    divider.shiftRight(1);
                    modulus.shiftRight(1);
                    if (divider <= dividend)
                        break;
                }
            }
            // dividend - it is equal to remainder of division

            return std::make_pair(quotient, dividend);
        }
        const_reference shiftLeft(unsigned int count);
        const_reference shiftRight(unsigned int count);

        //-------------Converter---------------//

        std::string to_string(unsigned int base = 10) const;

        //------------Setters, Getters----------//

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

        const_reference operator= (std::string_view numberStr) {
            this->assignStr(numberStr);
            return *this;
        }

        const_reference operator= (IntegralType small) {
            number_[0] = small;
            std::fill(std::next(number_.begin()), number_.end(), zeroIntegral());
            return *this;
        }

    public:
        void assignStr(std::string_view numberStr, unsigned int base = 10);

        //-------------------------Comparison---------------------------//

    private:
        template <LengthType lengthFirst, LengthType lengthSecond>
        bool isLess(LongUnsigned<lengthFirst> const& first,
                    LongUnsigned<lengthSecond> const& second) const
        {
            using FirstTypeIter = typename LongUnsigned<lengthFirst>::iterator;
            using SecondTypeIter = typename LongUnsigned<lengthSecond>::iterator;

            bool isLessVar = true;
            bool isEqual = true;
            bool isResultDefined = false;
            auto iterF = first.crbegin();
            auto iterS = second.crbegin();
            auto checkHigherPartToZero =
                [&isLessVar, &isEqual, &isResultDefined](auto& iter, const int lenHigh, const int lenLow)
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
                    if (*iterF > * iterS) {
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
            return (!isEqual) && isLessVar;
        }

    public:
        template <LengthType otherCount>
        bool operator!= (LongUnsigned<otherCount> const& other) const {
            bool isEqual = true;
            auto iter = cbegin();
            auto iterO = other.cbegin();
            for (; iter != cend() && iterO != other.cend(); iter++, iterO++) {
                if (*iter != *iterO) {
                    isEqual = false;
                    break;
                }
            }
            if (isEqual) { 
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
            return !isEqual;
        }
        template <LengthType otherCount>
        bool operator== (LongUnsigned<otherCount> const& other) const { return !(*this != other); }
        template <LengthType otherCount>
        bool operator< (LongUnsigned<otherCount> const& other) const { return this->isLess(*this, other); }

        template <LengthType otherCount>
        bool operator>= (LongUnsigned<otherCount> const& other) const { return !(*this < other); }
        template <LengthType otherCount>
        bool operator> (LongUnsigned<otherCount> const& other) const { return this->isLess(other, *this); }
        template <LengthType otherCount>
        bool operator<= (LongUnsigned<otherCount> const& other) const { return !(*this > other); }

    public:
        auto majorBitPosition() const
            -> std::optional<size_t>
        {
            for (int i = int(length()) - 1; i >= 0; i--) {
                auto curr = (*this)[i];
                if (curr > 0) {
                    size_t bitpos = 0;
                    while (curr > 0) {
                        curr >>= 1;
                        bitpos++;
                    }
                    return (bitpos - 1) + size_t(i) * 8 * sizeof(IntegralType);
                }
            }
            return std::nullopt;
        }

    public:
        LongUnsigned multiplyByKaracuba(const_reference other);

    public:
        static LongUnsigned max() {
            LongUnsigned max(1);
            max.shiftLeft((unsigned int)(integralModulusDegree() * length() - 1));
            return max;
        }

    protected:
        iterator begin() { return number_.begin(); }
        iterator end() { return number_.end(); }
        reverse_iterator rbegin() { return number_.begin(); }
        reverse_iterator rend() { return number_.end(); }
        const_iterator cbegin() const { return number_.cbegin(); }
        const_iterator cend() const { return number_.cend(); }
        const_reverse_iterator crbegin() const { return number_.crbegin(); }
        const_reverse_iterator crend() const { return number_.crend(); }

    private:
        // if index is increased then rank is increased
        array<IntegralType, countOfIntegrals> number_;

    };

    //------------------------------------------------------------------------------------------//
    //-------------------------------      Methods     -----------------------------------------//
    //------------------------------------------------------------------------------------------//

    template <LengthType countOfIntegrals>
    LongUnsigned<countOfIntegrals>::LongUnsigned(std::string_view numberStr, unsigned int base)
    {
        if (numberStr.length() <= 0 || base < 2)
            LongUnsigned();
        else
            assignStr(numberStr, base);
    }

    template <LengthType countOfIntegrals>
    void LongUnsigned<countOfIntegrals>::assignStr(std::string_view numberStr, unsigned int base) {
        const int integralModulusDegreeOfBase =
            int(std::log(2) / std::log(base) * integralModulusDegree());

        std::string_view numStrView = numberStr;
        numStrView.remove_prefix(
            cutOffLeftBorder<int>(0, int(numStrView.find_first_not_of(" ")))
        );

        // TODO: add exception for zero length
        if (numberStr.length() > 0 && base > 1) {
            // round the number by integral modulus
            numStrView.remove_prefix(
                cutOffLeftBorder<int>(0, int(numStrView.length()) - int(integralModulusDegreeOfBase * length()))
            );

            int blockLen = (base == 2) ? integralModulusDegreeOfBase - 1 : integralModulusDegreeOfBase;
            int last = int(numStrView.length());        // last variable is not included into segment [0, len - 1]
            int first = cutOffLeftBorder<int>(last - blockLen, 0);
            LongUnsigned<length()> iBase = 1;
            int subInt;

            std::fill(begin(), end(), zeroIntegral());
            while (last - first > 0) {
                auto sub = numStrView.substr(size_t(first), size_t(last) - size_t(first));
                std::from_chars(sub.data(), sub.data() + sub.size(), subInt, base);

                subInt = std::abs(subInt);
                auto jBase = iBase;
                while (subInt > 0) {
                    (*this) += LongUnsigned<1>(subInt % base) * jBase;
                    jBase *= LongUnsigned<1>(base);
                    subInt /= base;
                }

                iBase *= special::pow< LongUnsigned<1>, int, LongUnsigned<length()> >
                    (LongUnsigned<1>(base), last - first);
                last -= blockLen;
                first = cutOffLeftBorder<int>(first - blockLen, 0);
            }
        }
    }

    //------------Arithmetic Operations-------------//

    template <LengthType countOfIntegrals>
    auto LongUnsigned<countOfIntegrals>::shiftLeft(unsigned int count)
        -> const_reference
    {
        // 1    0    - indices
        // 1234 5678 - number
        if (count >= length() * integralModulusDegree()) {
            *this = 0;
        }
        else {
            auto& current = *this;
            int blocksShift = count / integralModulusDegree();
            int bitsShift = count % integralModulusDegree();    // 0 to 31
            for (int i = int(length()) - 1; i >= 0; i--) {
                auto high = (i - blocksShift < 0) ? 0 : (current[i - blocksShift] << bitsShift);
                auto less = (i - blocksShift - 1 < 0) ? 0 :
                    ((current[i - blocksShift - 1] >> (integralModulusDegree() - bitsShift - 1))
                        >> 1);      // INFO: this crutch must be because you cannot shift uint32_t >> 32 bits
                                    //       only 0 to 31.
                auto res = high | less;
                current[i] = res;
            }
        }
        return *this;
    }

    template <LengthType countOfIntegrals>
    auto LongUnsigned<countOfIntegrals>::shiftRight(unsigned int count)
        -> const_reference
    {
        if (count >= length() * integralModulusDegree()) {
            *this = 0;
        }
        else {
            auto& current = *this;
            int blocksShift = count / integralModulusDegree();
            int bitsShift = count % integralModulusDegree();    // 0 to 31
            for (size_t i = 0; i < length(); i++) {
                auto less = (i + blocksShift >= length())
                    ? 0 : (current[i + blocksShift] >> bitsShift);
                auto high = (i + blocksShift + 1 >= length())
                    ? 0 : ((current[i + blocksShift + 1] << (integralModulusDegree() - bitsShift - 1))
                        << 1);      // INFO: this crutch must be because you cannot shift uint32_t << 32 bits
                                    //       only 0 to 31.
                auto res = high | less;
                current[i] = res;
            }
        }
        return *this;
    }

    
    
    
    
    //----------------------------------------------------------------------------

    template <size_t countOfIntegrals>
    auto LongUnsigned<countOfIntegrals>::multiplyByKaracuba(const_reference other) 
        -> LongUnsigned<countOfIntegrals>
    {
        using OneIntegral = LongUnsigned<1>;
        using TwoIntegral = LongUnsigned<2>;

        auto& that = *this;
        LongUnsigned partialSum(0);

        if constexpr (countOfIntegrals % 2 == 0) {
            for (size_t i = 0; i < length(); i++) {
                partialSum -= OneIntegral(that[i] * other[i]);
            }
        }
        else {
            for (size_t i = 0; i < length(); i++) {
                partialSum += OneIntegral(that[i] * other[i]);
            }
        }

        LongUnsigned crossMul(0);
        if (countOfIntegrals > 1) {
            crossMul = (that[0] + that[1]) * (other[0] + other[1]);
        }

        LongUnsigned res = crossMul + partialSum;
        return res;
    }







    template <size_t countOfIntegrals>
    std::string LongUnsigned<countOfIntegrals>::to_string(unsigned int base) const {
        std::string res = "";
        LongUnsigned temp = *this;
        int i = 0;
        //constexpr size_t digitsCount = extra::getIntegralModulusDegree<rank>();
        do {
            auto pair = temp.divide(LongUnsigned(base));
            temp = pair.first;
            res += std::to_string((pair.second)[0]);
            i++;
        } while (temp > LongUnsigned<1>(0));
        return std::string(res.rbegin(), res.rend());
    }


    template <size_t countOfIntegrals>
    std::ostream& operator<<(std::ostream & out, LongUnsigned<countOfIntegrals> number) {
        return (out << number.to_string());
    }


}
