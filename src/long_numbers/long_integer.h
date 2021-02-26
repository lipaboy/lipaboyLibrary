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

	namespace extra {

		template <class TWord, class TSign>
		TSign sign(bool isNegative, TWord const& word) {
			return (isNegative * TSign(-1) + !isNegative * TSign(1))
				* (word != TWord(0));
		}

	}

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

		using TUnsignedPart = LongUnsigned<countOfIntegrals>;
		using IntegralType = typename TUnsignedPart::IntegralType;
		using ResultIntegralType = typename TUnsignedPart::ResultIntegralType;

        using ContainerType = std::array<IntegralType, countOfIntegrals>;
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
        
        LongInteger() {}

        LongInteger(TSigned small) {
            magnitude[0] = IntegralType(small);
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
//			else
//				assignStr(signedNumberStr, base);
		}

		//--------------------Arithmetic operations---------------------//

		template <LengthType first, LengthType second>
		using LongIntegerResult = LongInteger< extra::Max<first, second>::value >;

//		template <LengthType otherLen>
//		auto operator+(LongInteger<otherLen> const& other) const
//			-> LongIntegerResult<countOfIntegrals, otherLen>
//		{
//			using ResultType = LongIntegerResult<countOfIntegrals, otherLen>;
//			return (ResultType(*this) += other);
//		}

//		template <LengthType otherLen>
//		auto operator+=(LongInteger<otherLen> const& other)
//			-> const_reference
//		{
//			// About implicit conversation from signed to unsigned:
//			// https://stackoverflow.com/questions/50605/signed-to-unsigned-conversion-in-c-is-it-always-safe

//			constexpr auto MIN_LENGTH = std::min(countOfIntegrals, otherLen);
//			IntegralType carryOver(0);
//			TSignedResult carryOverSign(1);
//			constexpr TSignedResult mask(1);
//			const TSignedResult selfSign = this->signButZero();   // O(sign()) ~ O(N)
//			const TSignedResult otherSign = other.signButZero();
//			size_t i = 0;

//			// TODO: hide the type-conversation into something
//			for (; i < MIN_LENGTH; i++) {
//				const TSignedResult dualRes =
//					selfSign * TSignedResult(this->unsignedPart_[i])
//					+ otherSign * TSignedResult(other.unsignedPart_[i])
//					+ carryOverSign * TSignedResult(carryOver);

//				// std::abs - bad choice for extracting the result from dualRes (or not?)
//				carryOverSign =
//					extra::sign<TSignedResult, TSignedResult>(dualRes < 0, dualRes);
				
//				this->unsignedPart_[i] =
//					IntegralType(dualRes & TSignedResult(integralModulus()));
//				carryOver =
//					IntegralType(mask & (dualRes >> integralModulusDegree()));
//			}
//			if constexpr (length() > MIN_LENGTH) {
//				for (; i < length(); i++) {
//					const TSignedResult dualRes =
//						selfSign * TSignedResult(this->unsignedPart_[i])
//						+ carryOverSign * TSignedResult(carryOver);

//					this->unsignedPart_[i] =
//						IntegralType(dualRes & TSignedResult(integralModulus()));
//					carryOver =
//						IntegralType(mask & (dualRes >> integralModulusDegree()));
//					carryOverSign =
//						extra::sign<TSignedResult, TSignedResult>(dualRes < 0, dualRes);
//				}
//			}
//			this->setSign(TSigned(carryOverSign));

//			return *this;
//		}

	public:
//		void setSign(TSigned newSign) { minus_ = newSign < 0; }

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

	private:
		// Complexity: O(1) - because without checking on zero value
        TSigned signExceptZero() const {
            return TSigned(((magnitude.back() & (1u << (integralModulusDegree() - 1))) != 0) ? -1 : 1);
        }

	public:
        LongInteger operator-() const
        {
            using TResult = ResultIntegralType;

            LongInteger inverted;
            auto itOut = inverted.begin();
            IntegralType remainder = 1;
            for (auto itIn = cbegin(); itIn != cend(); itIn++) {
                TResult res = TResult(~(*itIn)) + TResult(remainder);

                *(itOut++) = IntegralType(res & integralModulus());
                remainder = IntegralType(res >> integralModulusDegree());
            }

            return inverted;
        }

		//----------------------------Utils------------------------------//

//		void assignStr(std::string_view signedNumberStr, unsigned int base = 10);

//		std::string to_string(unsigned int base = 10) const {
//            return ((sign() < 0) ? "-" : "") + unsignedPart_.to_string(base);
//		}

    protected:
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

//	template <LengthType length>
//	void LongInteger<length>::assignStr(std::string_view signedNumberStr, unsigned int base) {
//		if (base > 1) {
//			std::string_view signedNumberStrView = signedNumberStr;
//			signedNumberStrView.remove_prefix(
//				cutOffLeftBorder<int>(0, int(signedNumberStrView.find_first_not_of(" ")))
//			);

//			// TODO: add exception for zero length
//			if (signedNumberStrView.length() > 0) {
//				minus_ = (signedNumberStrView.front() == '-') ? true : false;
//				if (minus_)
//					signedNumberStrView.remove_prefix(1);
//				unsignedPart_.assignStr(signedNumberStrView, base);
//			}
//		}
//	}

}

