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
        // See LongIntegerDecimal

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

        namespace extra1 {

            template <class TWord>
            inline constexpr size_t bitsCount() { return sizeof(TWord) * 8; }

            template <class TWord, class TSign>
            TSign sign(bool isNegative, TWord const& word) {
                return (isNegative * TSign(-1) + !isNegative * TSign(1))
                    * (word != TWord(0));
            }

            //////////////////////////////////////////////////////////////////////////////////
            template <size_t val1, size_t val2>
            struct Max {
                static constexpr size_t value = (val1 < val2) ? val2 : val1;
            };

            template <size_t rank>
            inline constexpr size_t getIntegralModulusDegree() {
                return static_cast<size_t>(std::floor(
                    std::log(2) / std::log(rank) * double(bitsCount<uint32_t>())));
            }

            using LengthType = size_t;
        }


        using extra1::LengthType;

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
            using LengthType = extra1::LengthType;
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
            LongUnsigned(ContainerType const& number, bool minus)
                : number_(number), minus_(minus)
            {}

        public:
            // Note: Non-initialized constructor: without filling array by zeroIntegral value.
            explicit
                LongUnsigned() { checkTemplateParameters(); }
            LongUnsigned(int small) : minus_(small < 0) {
                checkTemplateParameters();
                number_[0] = std::abs(small);
                std::fill(std::next(begin()), end(), TIntegral(0));
            }
            explicit
                LongUnsigned(string const& numberDecimalStr);

            // TODO: calculate how much copy-constructor was called
            LongUnsigned operator+(const_reference other) const { 
                return (LongUnsigned(*this) += other); 
            }

            const_reference operator+=(const_reference other);

            // TODO: you can optimize it. When inverse operator is called then useless copy will be created.
            const_reference operator-=(const_reference other);

            LongUnsigned operator-(const_reference other) const {
                return (LongUnsigned(*this) -= other);
            }

            template <LengthType length2>
            auto operator*(LongUnsigned<length2> const& other) const
                -> LongUnsigned<extra1::Max<lengthOfIntegrals, length2>::value >
            {
                using ResultType = LongUnsigned<extra1::Max<lengthOfIntegrals, length2>::value >;
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
            // maximum count decimal digits that can be placed into IntegralType
            static constexpr IntegralType integralModulusDegree() { return extra1::bitsCount<IntegralType>(); }
            static constexpr ResultIntegralType integralModulus() { return std::numeric_limits<IntegralType>::max(); }
            static constexpr size_type maxDigitsCount() { return length() * integralModulusDegree(); }

        private:
            static constexpr IntegralType zeroIntegral() { return IntegralType(0); }

        public:
            // Return the remainder of division by 10
            //IntegralType divideByDec();

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

        };

        //-------------------------------------------------------------//

        template <LengthType length>
        LongUnsigned<length>::LongUnsigned(string const& numberDecimalStr) 
            : minus_(false) 
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
                    while (subInt > 0) {
                        (*this) += LongUnsigned(subInt % base) * iBase;
                        iBase *= LongUnsigned(base);
                        subInt /= base;
                    }

                    last -= integralModulusDegreeOfBase;
                    first = cutOffLeftBorder<int>(first - integralModulusDegreeOfBase, 0);
                }
            }
        }

        //------------Arithmetic Operations-------------//

        template <LengthType length>
        auto LongUnsigned<length>::operator+=(const_reference other)
            -> const_reference
        {
            // Think_About: maybe std::partial_sum can be useful?

            IntegralType remainder = zeroIntegral();
            TSigned sign(1);
            for (size_t i = 0; i < length(); i++) {
                const TIntegralResult doubleTemp =
                    TIntegralResult((*this)[i])
                    + TIntegralResult(other[i])
                    + TIntegralResult(remainder);

                (*this)[i] = IntegralType(doubleTemp & integralModulus());
                remainder = IntegralType(doubleTemp >> integralModulusDegree());
            }

            return *this;
        }

        template <LengthType length>
        auto LongUnsigned<length>::operator-=(const_reference other)
            -> const_reference
        {
            // Think_About: maybe std::partial_sum can be useful?

            IntegralType remainder = zeroIntegral();
            TSigned sign(1);
            for (size_t i = 0; i < length(); i++) {
                const TIntegralResult doubleTemp =
                    TIntegralResult((*this)[i])
                    - TIntegralResult(other[i])
                    - TIntegralResult(remainder);

                (*this)[i] = IntegralType(doubleTemp & integralModulus());
                remainder = IntegralType(doubleTemp >> (2 * integralModulusDegree() - 1));
            }

            return *this;
        }

        template <LengthType length>
        auto LongUnsigned<length>::divide(const_reference other) const
            -> pair<LongUnsigned, LongUnsigned>
        {
            // TODO: replace to OneDigitNumber
            const LongUnsigned DEC(10);
            const LongUnsigned ONE(1);
            const LongUnsigned ZERO(0);

            LongUnsigned divider(other);
            LongUnsigned res(0);
            LongUnsigned dividend(*this);

            LongUnsigned modulus(1);
            // TODO: replace infinite loop to smth else
            while (true) {
                divider.shiftLeft(1);
                if (divider > dividend)
                    break;
                modulus.shiftLeft(1);
            }

            divider.shiftRight(1);
            while (dividend >= divider || modulus != ONE) {
                while (dividend >= divider) {
                    dividend -= divider;
                    res += modulus;
                }

                while (modulus != ONE) {
                    divider.shiftRight(1);
                    modulus.shiftRight(1);
                    if (divider <= dividend)
                        break;
                }
            }
            // dividend - it is equal to remainder of division

            return std::make_pair(res, dividend);
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
                int bitsShift = count % integralModulusDegree();
                for (int i = length() - 1; i >= 0; i--) {
                    auto high = (i - blocksShift < 0) ? 0 : (current[i - blocksShift] << bitsShift);
                    auto less = (i - blocksShift - 1 < 0) ? 0 : 
                        (current[i - blocksShift - 1] >> (integralModulusDegree() - bitsShift));
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
                int bitsShift = count % integralModulusDegree();
                for (size_type i = 0; i < length(); i++) {
                    auto less = (i + blocksShift >= length()) 
                        ? 0 : (current[i + blocksShift] >> bitsShift);
                    auto high = (i + blocksShift + 1 >= length()) 
                        ? 0 : (current[i + blocksShift + 1] << (integralModulusDegree() - bitsShift));
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
            //constexpr size_t digitsCount = extra1::getIntegralModulusDegree<rank>();
            do {
                auto pair = temp.divide(LongUnsigned(base));
                temp = pair.first;
                res += std::to_string((pair.second)[0]);
            } while (temp > LongUnsigned<1>(0));
            return std::string(res.rbegin(), res.rend());
        }

        


    }

}
