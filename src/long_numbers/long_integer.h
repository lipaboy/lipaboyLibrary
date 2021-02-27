#pragma once

#include <cmath>    // std::abs
#include <array>

// building number from string
#include <string>
#include <string_view>
#include "intervals/cutoffborders.h"

#include "long_number_base.h"
#include "long_unsigned.h"

namespace lipaboy_lib::long_numbers_space {

    // TODO: create LongNumberBase for common methods: integralModulus, Degree, IntegralTypes, zeroIntegral
    //

    // LongInteger
    // Concept: use same logic as build into Integral types (signed int). Two's complement signed number representation

    using extra::LengthType;

    template <LengthType countOfIntegrals>
    class LongInteger :
        public LongNumberBase<uint32_t, uint64_t, countOfIntegrals>
    {
    public:
        using Sub = LongNumberBase<uint32_t, uint64_t, countOfIntegrals>;
        using TSigned = std::int32_t;
        using TSignedResult = std::int64_t;
        using MinusType = bool;
        using const_reference = LongInteger const&;

        using ContainerType = LongUnsigned<countOfIntegrals>;
        using IntegralType = typename ContainerType::IntegralType;
        using ResultIntegralType = typename ContainerType::ResultIntegralType;
        using iterator = typename ContainerType::iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_iterator = typename ContainerType::const_iterator;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        using Sub::length;
        using Sub::zeroIntegral;
        using Sub::unitsIntegral;
        using Sub::integralModulus;
        using Sub::integralModulusDegree;

    public:
        template <LengthType otherLengthOfIntegrals>
        friend class LongInteger;

    private:
        LongInteger(ContainerType magnitude_)
            : magnitude(magnitude_) {}

    public:

        LongInteger() : magnitude() {}

        LongInteger(TSigned small) {
            magnitude[0] = IntegralType(small);
            std::fill(std::next(begin()), end(), (small < 0) ? unitsIntegral() : zeroIntegral());
        }

        template <LengthType otherLen>
        explicit
            LongInteger(LongInteger<otherLen> const& other) {
                auto minLen = (other.length() > length()) ? length() : other.length();
                std::copy_n(other.cbegin(), minLen, std::begin(magnitude));
                if (other.signExceptZero() < 0)
                    std::fill(std::next(begin(), minLen), end(), unitsIntegral());
                else
                    std::fill(std::next(begin(), minLen), end(), zeroIntegral());
            }

        explicit
            LongInteger(std::string_view signedNumberStr, unsigned int base = 10)
        {
            if (signedNumberStr.length() <= 0 || base < 2)
                LongInteger();
            else
                assignStr(signedNumberStr, base);
        }

        //--------------------Arithmetic operations---------------------//

        template <LengthType first, LengthType second>
        using LongIntegerResult = LongInteger< extra::Max<first, second>::value >;

        template <LengthType otherLen>
        auto operator+(LongInteger<otherLen> const& other) const
            -> LongIntegerResult<countOfIntegrals, otherLen>
        {
            using ResultType = LongIntegerResult<countOfIntegrals, otherLen>;
            return (ResultType(*this) += other);
        }

        template <LengthType otherLen>
        auto operator+=(LongInteger<otherLen> const& other)
            -> const_reference
        {
            // About implicit conversation from signed to unsigned:
            // https://stackoverflow.com/questions/50605/signed-to-unsigned-conversion-in-c-is-it-always-safe

            using TResult = ResultIntegralType;

            constexpr auto MIN_LENGTH = std::min(countOfIntegrals, otherLen);
            IntegralType carryOver(0);
            size_t i = 0;
            for (; i < MIN_LENGTH; i++) {
                const TResult dualRes =
                    TResult(this->magnitude[i])
                    + TResult(other.magnitude[i])
                    + TResult(carryOver);

                this->magnitude[i] =
                    IntegralType(dualRes & TResult(integralModulus()));
                carryOver =
                    IntegralType(dualRes >> integralModulusDegree());
            }
            if constexpr (length() > MIN_LENGTH) {
                IntegralType otherSignRemainder =
                        (other.isNegative()) ? unitsIntegral() : zeroIntegral();
                for (; i < length(); i++) {
                    const TResult dualRes =
                        TResult(this->magnitude[i])
                        + TResult(otherSignRemainder)
                        + TResult(carryOver);

                    this->magnitude[i] =
                        IntegralType(dualRes & TResult(integralModulus()));
                    carryOver =
                        IntegralType(dualRes >> integralModulusDegree());
                }
            }

            return *this;
        }

        template <LengthType otherLen>
        auto operator-(LongInteger<otherLen> const& other) const
            -> LongIntegerResult<countOfIntegrals, otherLen>
        {
            return (*this + (-other));
        }

        template <LengthType otherLen>
        auto operator-=(LongInteger<otherLen> const& other)
            -> const_reference
        {
            return (*this += (-other));
        }

    public:
        // Complexity: O(N) - because isZero() method
        TSigned sign() const {
            return signExceptZero() * (!isZero());      // ~ O(N)
        }

        bool isZero() const {
            for (auto iter = cbegin(); iter != cend(); iter++)
                if (*iter != zeroIntegral())
                    return false;
            return true;
        }

        bool isNegative() const {
            return signExceptZero() < 0;    // O(1)
        }

    private:
        // Complexity: O(1) - because without checking on zero value
        TSigned signExceptZero() const {
            return TSigned(((magnitude.back() & (1u << (integralModulusDegree() - 1))) != 0) ? -1 : 1);
        }

    public:
        const_reference invert()
        {
            using TResult = ResultIntegralType;

            IntegralType carryOver = 1;
            for (auto iter = begin(); iter != end(); iter++) {
                TResult res = TResult(~(*iter)) + TResult(carryOver);

                *iter = IntegralType(res & integralModulus());
                carryOver = IntegralType(res >> integralModulusDegree());
            }

            return *this;
        }

        LongInteger operator-() const
        {
            using TResult = ResultIntegralType;

            LongInteger inverted;
            auto itOut = inverted.begin();
            IntegralType carryOver = 1;
            for (auto itIn = cbegin(); itIn != cend(); itIn++) {
                TResult res = TResult(~(*itIn)) + TResult(carryOver);

                *(itOut++) = IntegralType(res & integralModulus());
                carryOver = IntegralType(res >> integralModulusDegree());
            }

            return inverted;
        }

        //----------------------------Utils------------------------------//

        void assignStr(std::string_view signedNumberStr, unsigned int base = 10);

        std::string to_string(unsigned int base = 10) const {
            return (isNegative()) ? "-" + (-(*this)).magnitude.to_string(base) : magnitude.to_string(base);
        }

    public:
        iterator begin() { return magnitude.begin(); }
        iterator end() { return magnitude.end(); }
        reverse_iterator rbegin() { return magnitude.begin(); }
        reverse_iterator rend() { return magnitude.end(); }
        const_iterator cbegin() const { return magnitude.cbegin(); }
        const_iterator cend() const { return magnitude.cend(); }
        const_reverse_iterator crbegin() const { return magnitude.crbegin(); }
        const_reverse_iterator crend() const { return magnitude.crend(); }

    private:
        ContainerType magnitude;
    };

    template <LengthType length>
    void LongInteger<length>::assignStr(std::string_view signedNumberStr, unsigned int base) {
        if (base > 1) {
            std::string_view signedNumberStrView = signedNumberStr;
            signedNumberStrView.remove_prefix(
                cutOffLeftBorder<int>(0, int(signedNumberStrView.find_first_not_of(" ")))
            );

            // TODO: add exception for zero length
            if (signedNumberStrView.length() > 0) {
                bool minus = (signedNumberStrView.front() == '-') ? true : false;
                if (minus)
                    signedNumberStrView.remove_prefix(1);
                magnitude.assignStr(signedNumberStrView, base);
                if (minus)
                    (*this) = -(*this);
            }
        }
    }

}

