#pragma once

#include <cmath>    // std::abs

// building number from string
#include <string>
#include <string_view>
#include "intervals/cutoffborders.h"

#include "long_unsigned.h"

namespace lipaboy_lib::long_numbers_space {

	using extra::LengthType;

	template <LengthType lengthOfIntegrals>
	class LongInteger
	{
	public:
		using TSigned = std::int32_t;
		using TSignedResult = std::int64_t;
		using MinusType = bool;

		using TUnsignedPart = LongUnsigned<lengthOfIntegrals>;

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

        TSigned sign() const {
            return (minus_ * TSigned(-1) + !minus_ * TSigned(1)) * (!unsignedPart_.isZero());
        }

		LongInteger operator-() const {
			return LongInteger(unsignedPart_, !minus_);
		}

		//--------------------Class characteristics----------------------//

		static constexpr LengthType length() { return lengthOfIntegrals; }

		//----------------------------Utils------------------------------//

		void assignStr(std::string_view signedNumberStr, unsigned int base = 10);

		std::string to_string(unsigned int base = 10) const {
			return ((minus_) ? "-" : "") + unsignedPart_.to_string();
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

