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
#include <optional>

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
        // See LongIntegerDecimal
        // 1) Write new Algebra with another realization: instead of use method 'getNumber()'
        //      for wrapper class you need to cast the primary type to wrapper class.
        //      Like: (*this) + LongUnsigned(value)

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

        ////////////////////////////////////////////////////////////////////////////////////////

        namespace extra {

            template <class TWord>
            inline constexpr size_t bitsCount() { return sizeof(TWord) * 8; }

            //////////////////////////////////////////////////////////////////////////////////
            template <size_t val1, size_t val2>
            struct Max {
                static constexpr size_t value = (val1 < val2) ? val2 : val1;
            };

            using LengthType = size_t;
        }


        using extra::LengthType;

        // Requirements: 
        // 1) TIntegral and TResult must be unsigned.

        template <LengthType lengthOfIntegrals>     // count of integral type variables
        class LongUnsigned
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

            using reference = LongUnsigned&;
            using const_reference = const LongUnsigned&;
            using reference_integral = IntegralType&;
            using const_reference_integral = const IntegralType&;

        protected:
            LongUnsigned(ContainerType const& number)
                : number_(number)
            {}

        public:
            // Note: Non-initialized constructor: without filling array by zeroIntegral value.
            explicit
                LongUnsigned() { checkTemplateParameters(); }
            LongUnsigned(IntegralType small) {
                checkTemplateParameters();
                number_[0] = small;
                std::fill(std::next(begin()), end(), zeroIntegral());
            }
            explicit
                LongUnsigned(string const& numberDecimalStr);

            // TODO: calculate how much copy-constructor was called
            template <LengthType length2>
            LongUnsigned operator+(LongUnsigned<length2> const& other) const {
                return (LongUnsigned(*this) += other); 
            }

            template <LengthType length2>
            auto operator+=(LongUnsigned<length2> const & other)
                -> const_reference
            {
                constexpr auto MIN_LENGTH = std::min(lengthOfIntegrals, length2);
                IntegralType remainder = zeroIntegral();
                size_t i = 0;
                for (; i < MIN_LENGTH; i++) {
                    const TIntegralResult dualTemp =
                        TIntegralResult((*this)[i])
                        + TIntegralResult(other[i])
                        + TIntegralResult(remainder);

                    (*this)[i] = IntegralType(dualTemp & integralModulus());
                    remainder = IntegralType(dualTemp >> integralModulusDegree());
                }
                if constexpr (length() > MIN_LENGTH) {
                    for (; i < length(); i++) {
                        const TIntegralResult dualTemp =
                            TIntegralResult((*this)[i])
                            + TIntegralResult(remainder);

                        (*this)[i] = IntegralType(dualTemp & integralModulus());
                        remainder = IntegralType(dualTemp >> integralModulusDegree());
                    }
                }

                return *this;
            }

            // TODO: you can optimize it. When inverse operator is called then useless copy will be created.
            template <LengthType length2>
            LongUnsigned operator-(LongUnsigned<length2> const& other) const {
                return (LongUnsigned(*this) -= other);
            }

            template <LengthType length2>
            auto operator-=(LongUnsigned<length2> const& other)
                -> const_reference
            {
                constexpr auto MIN_LENGTH = std::min(lengthOfIntegrals, length2);
                IntegralType remainder = zeroIntegral();
                size_t i = 0;
                for (; i < MIN_LENGTH; i++) {
                    const TIntegralResult dualTemp =
                        TIntegralResult((*this)[i])
                        - TIntegralResult(other[i])
                        - TIntegralResult(remainder);

                    (*this)[i] = IntegralType(dualTemp & integralModulus());
                    remainder = IntegralType(dualTemp >> (2 * integralModulusDegree() - 1));
                }
                if constexpr (length() > MIN_LENGTH) {
                    for (; i < length(); i++) {
                        const TIntegralResult dualTemp =
                            TIntegralResult((*this)[i])
                            - TIntegralResult(remainder);

                        (*this)[i] = IntegralType(dualTemp & integralModulus());
                        remainder = IntegralType(dualTemp >> (2 * integralModulusDegree() - 1));
                    }
                }

                return *this;
            }

            template <LengthType length2>
            auto operator*(LongUnsigned<length2> const& other) const
                -> LongUnsigned<extra::Max<lengthOfIntegrals, length2>::value >
            {
                using ResultType = LongUnsigned<extra::Max<lengthOfIntegrals, length2>::value >;
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

                        const TIntegralResult dualTemp = TIntegralResult((*this)[i]) * other[j] + remainder;
                        // Detail #2
                        res[i + j] += IntegralType(dualTemp & integralModulus());
                        remainder = IntegralType(dualTemp >> integralModulusDegree());
                    }
                }

                return res;
            }

            template <LengthType length2>
            const_reference operator*=(LongUnsigned<length2> const& other) {
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

            auto divide(const_reference other) const->pair<LongUnsigned, LongUnsigned>;

            const_reference shiftLeft(unsigned int count);
            const_reference shiftRight(unsigned int count);

            //-------------Converter---------------//

            string to_string(unsigned int base = 10) const;

            //------------Setters, Getters----------//

            static constexpr LengthType length() { return lengthOfIntegrals; }

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

            const_reference operator= (IntegralType small) {
                number_[0] = small;
                std::fill(std::next(number_.begin()), number_.end(), zeroIntegral());
                return *this;
            }

        private:
            void assignString(string const& numberDecimalStr, unsigned int base = 10);

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
            template <LengthType lengthOther>
            bool operator!= (LongUnsigned<lengthOther> const& other) const {
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
                return !isEqual;
            }
            template <LengthType lengthOther>
            bool operator== (LongUnsigned<lengthOther> const& other) const { return !(*this != other); }
            template <LengthType lengthOther>
            bool operator< (LongUnsigned<lengthOther> const& other) const { return this->isLess(*this, other); }

            template <LengthType lengthOther>
            bool operator>= (LongUnsigned<lengthOther> const& other) const { return !(*this < other); }
            template <LengthType lengthOther>
            bool operator> (LongUnsigned<lengthOther> const& other) const { return this->isLess(other, *this); }
            template <LengthType lengthOther>
            bool operator<= (LongUnsigned<lengthOther> const& other) const { return !(*this > other); }

        public:
            auto majorBitPosition() const 
                -> std::optional<size_type>
            {
                for (int i = int(length()) - 1; i >= 0; i--) {
                    auto curr = (*this)[i];
                    if (curr > 0) {
                        size_type bitpos = 0;
                        while (curr > 0) {
                            curr >>= 1;
                            bitpos++;
                        }
                        return (bitpos - 1) + i * 8 * sizeof(IntegralType);
                    }
                }
                return std::nullopt;
            }

        public:
            // maximum count decimal digits that can be placed into IntegralType
            static constexpr IntegralType integralModulusDegree() { return extra::bitsCount<IntegralType>(); }
            static constexpr ResultIntegralType integralModulus() { return std::numeric_limits<IntegralType>::max(); }
            static constexpr size_type maxDigitsCount() { return length() * integralModulusDegree(); }

        private:
            static constexpr IntegralType zeroIntegral() { return IntegralType(0); }

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
        };

        //-------------------------------------------------------------//

        template <LengthType length>
        LongUnsigned<length>::LongUnsigned(string const& numberDecimalStr) 
        {
            checkTemplateParameters();
            if (numberDecimalStr.length() <= 0)
                LongUnsigned();
            else
                assignString(numberDecimalStr);
        }

        template <LengthType length>
        void LongUnsigned<length>::assignString(string const& numberDecimalStr, unsigned int base) {
            // TODO: add exception for zero length
            if (numberDecimalStr.length() > 0) {
                std::string_view numStrView = numberDecimalStr;
                numStrView.remove_prefix(
                    cutOffLeftBorder<int>(0, numStrView.find_first_not_of(" "))
                );
                // round the number by integral modulus
                numStrView.remove_prefix(
                    cutOffLeftBorder<int>(0, TSigned(numStrView.length()) - TSigned(maxDigitsCount()))
                );

                const int integralModulusDegreeOfBase = 
                    int(std::log(2) / std::log(base) * integralModulusDegree());
                int last = int(numStrView.length());
                int first = cutOffLeftBorder<int>(last - integralModulusDegreeOfBase, 0);
                size_t i = 0;
                LongUnsigned iBase = 1;
                int subInt;

                std::fill(begin(), end(), zeroIntegral());
                for (; last - first > 0 && i < length(); i++) {
                    auto sub = numStrView.substr(size_t(first), size_t(last) - size_t(first));
                    std::from_chars(sub.data(), sub.data() + sub.size(), subInt);

                    subInt = std::abs(subInt);
                    LongUnsigned jBase = iBase;
                    while (subInt > 0) {
                        (*this) += LongUnsigned(subInt % base) * jBase;
                        jBase *= LongUnsigned(base);
                        subInt /= base;
                    }

                    iBase *= special::pow<LongUnsigned<1>, int, LongUnsigned>(LongUnsigned<1>(base), last - first);
                    last -= integralModulusDegreeOfBase;
                    first = cutOffLeftBorder<int>(first - integralModulusDegreeOfBase, 0);
                }
            }
        }

        //------------Arithmetic Operations-------------//

        template <LengthType length>
        auto LongUnsigned<length>::divide(const_reference other) const
            -> pair<LongUnsigned, LongUnsigned>
        {
            // TODO: replace to OneDigitNumber
            const LongUnsigned<1> DEC(10);
            const LongUnsigned<1> ONE(1);
            const LongUnsigned<1> ZERO(0);

#if (defined(WIN32) && defined(DEBUG_)) || (defined(__linux__) && !defined(NDEBUG))
            // TODO: replace to throw exception
            if (other == ZERO) {
                throw std::runtime_error("Runtime Error (LongUnsigned): division by zero");
            }
#endif

            LongUnsigned quotient(0);
            LongUnsigned dividend(*this);
            LongUnsigned divider(other);
            LongUnsigned modulus(1);

            int dividendMajorBit = dividend.majorBitPosition().value_or(0);
            int dividerMajorBit = divider.majorBitPosition().value_or(0);

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

        // TODO: test it
        template <LengthType length>
        auto LongUnsigned<length>::shiftLeft(unsigned int count)
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
                for (int i = length() - 1; i >= 0; i--) {
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

        template <LengthType length>
        auto LongUnsigned<length>::shiftRight(unsigned int count)
            -> const_reference
        {
            if (count >= length() * integralModulusDegree()) {
                *this = 0;
            }
            else {
                auto& current = *this;
                int blocksShift = count / integralModulusDegree();
                int bitsShift = count % integralModulusDegree();    // 0 to 31
                for (size_type i = 0; i < length(); i++) {
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

        /*template <LengthType length>
        auto LongUnsigned<length>::divideByDec()
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
        }*/
        

        //----------------------------------------------------------------------------

        template <size_t length>
        string LongUnsigned<length>::to_string(unsigned int base) const {
            string res = "";
            LongUnsigned temp = *this;
            int i = 0;
            //constexpr size_t digitsCount = extra::getIntegralModulusDegree<rank>();
            do {
                if (i == 61) {
                    cout << "kek";
                }
                auto pair = temp.divide(LongUnsigned(base));
                temp = pair.first;
                res += std::to_string((pair.second)[0]);
                i++;
            } while (temp > LongUnsigned<1>(0));
            return std::string(res.rbegin(), res.rend());
        }




    }

}
