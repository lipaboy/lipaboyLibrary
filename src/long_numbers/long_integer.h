#pragma once

#include <cmath>    // std::abs

// building number from string
#include <string>
#include <string_view>
#include "intervals/cutoffborders.h"

#include "long_number_base.h"
#include "long_unsigned.h"

namespace lipaboy_lib::long_numbers_space {

	// TODO: create LongNumberBase for common methods: integralModulus, Degree, IntegralTypes, zeroIntegral
	//			

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
		using TSigned = std::int32_t;
		using TSignedResult = std::int64_t;
		using MinusType = bool;
		using const_reference = LongInteger const&;

		using TUnsignedPart = LongUnsigned<countOfIntegrals>;
		using IntegralType = typename TUnsignedPart::IntegralType;
		using ResultIntegralType = typename TUnsignedPart::ResultIntegralType;

	public:
		template <LengthType otherLengthOfIntegrals>
		friend class LongInteger;

	private:
		LongInteger(TUnsignedPart unsignedPart, MinusType minus)
			: unsignedPart_(unsignedPart), minus_(minus) {}

	public:
        
        LongInteger() : unsignedPart_(), minus_(false) {}

		LongInteger(TSigned small) : unsignedPart_(std::abs(small)), minus_(small < 0) {}

		template <LengthType otherLen>
		explicit
			LongInteger(LongInteger<otherLen> const& other) 
				: unsignedPart_(other.unsignedPart_), minus_(other.minus_) {}

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

			constexpr auto MIN_LENGTH = std::min(countOfIntegrals, otherLen);
			IntegralType carryOver(0);
			TSignedResult carryOverSign(1);
			constexpr TSignedResult mask(1);
			const TSignedResult selfSign = this->signButZero();   // O(sign()) ~ O(N)
			const TSignedResult otherSign = other.signButZero();
			size_t i = 0;

			// TODO: hide the type-conversation into something
			for (; i < MIN_LENGTH; i++) {
				const TSignedResult dualRes =
					selfSign * TSignedResult(this->unsignedPart_[i])
					+ otherSign * TSignedResult(other.unsignedPart_[i])
					+ carryOverSign * TSignedResult(carryOver);

				// std::abs - bad choice for extracting the result from dualRes (or not?)
				carryOverSign =	
					extra::sign<TSignedResult, TSignedResult>(dualRes < 0, dualRes);
				
				this->unsignedPart_[i] = 
					IntegralType(dualRes & TSignedResult(integralModulus()));
				carryOver =	
					IntegralType(mask & (dualRes >> integralModulusDegree()));
			}
			if constexpr (length() > MIN_LENGTH) {
				for (; i < length(); i++) {
					const TSignedResult dualRes =
						selfSign * TSignedResult(this->unsignedPart_[i])
						+ carryOverSign * TSignedResult(carryOver);

					this->unsignedPart_[i] =
						IntegralType(dualRes & TSignedResult(integralModulus()));
					carryOver = 
						IntegralType(mask & (dualRes >> integralModulusDegree()));
					carryOverSign =	
						extra::sign<TSignedResult, TSignedResult>(dualRes < 0, dualRes);
				}
			}
			this->setSign(TSigned(carryOverSign));

			return *this;
		}

	public:
		void setSign(TSigned newSign) { minus_ = newSign < 0; }

		// Complexity: O(N) - because isZero() method
        TSigned sign() const {
            return (minus_ * TSigned(-1) + !minus_ * TSigned(1)) 
				* (!unsignedPart_.isZero());	// ~ O(N)
        }
	private:
		// Complexity: O(1) - because without checking on zero value
		TSigned signButZero() const {
			return (minus_ * TSigned(-1) + !minus_ * TSigned(1));
		}

	public:
		LongInteger operator-() const {
			return LongInteger(unsignedPart_, !minus_);
		}

		//----------------------------Utils------------------------------//

		void assignStr(std::string_view signedNumberStr, unsigned int base = 10);

		std::string to_string(unsigned int base = 10) const {
			return ((sign() < 0) ? "-" : "") + unsignedPart_.to_string();
		}

	private:
		TUnsignedPart unsignedPart_;
		MinusType minus_;
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
				minus_ = (signedNumberStrView.front() == '-') ? true : false;
				if (minus_)
					signedNumberStrView.remove_prefix(1);
				unsignedPart_.assignStr(signedNumberStrView, base);
			}
		}
	}

}

