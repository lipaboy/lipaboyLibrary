#pragma once

#include <cmath>    // std::abs

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

	private:
		LongInteger(TUnsignedPart unsignedPart, MinusType minus)
			: unsignedPart_(unsignedPart), minus_(minus) {}

	public:
        
        LongInteger() : unsignedPart_(), minus_(false) {}

        LongInteger(TSigned small) : unsignedPart_(std::abs(small)), minus_(small < 0) {}

		// copy-constructor for other length
		// constructor from string

        TSigned sign() const {
            return (minus_ * TSigned(-1) + !minus_ * TSigned(1)) * (!unsignedPart_.isZero());
        }

		LongInteger operator-() const {
			return LongInteger(unsignedPart_, !minus_);
		}

	private:
		TUnsignedPart unsignedPart_;
		MinusType minus_;
	};

}

