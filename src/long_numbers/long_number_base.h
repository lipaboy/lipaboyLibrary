#pragma once

#include "extra_tools/extra_tools.h"

namespace lipaboy_lib::long_numbers_space {

	// Description: class that combines basic tools for long-number classes 
	//		(like LongUnsigned and LongInteger)

    // Definitions:
    //      1) Integral - part of long-number; variable of Integral Type (like uint32_t) that
    //          stores part of number.

    namespace extra {

        template <class TWord>
        inline constexpr size_t bitsCount() { return sizeof(TWord) * 8; }

        template <size_t val1, size_t val2>
        struct Max {
            static constexpr size_t value = (val1 < val2) ? val2 : val1;
        };

        using LengthType = size_t;
    }

    using extra::LengthType;
    using lipaboy_lib::enable_if_else_t;

	template <class TIntegral, class TIntegralResult, LengthType countOfIntegrals>     // count of integral type variables
	class LongNumberBase {
	public:
        using LengthType = extra::LengthType;

        // INFO:
        // Checking if the result type equals the double of integral type. Example:
        // "res = a + b" - where res has TIntegralResult type, a and b have TIntegral type
        using IntegralType =
            std::remove_reference_t<
            enable_if_else_t<2 * sizeof(TIntegral) == sizeof(TIntegralResult), TIntegral, void> >;
        using ResultIntegralType = std::remove_reference_t<TIntegralResult>;

    public:
        LongNumberBase() {
            checkTemplateParameters();
        }

        //--------------------Class characteristics----------------------//
    public:
        static constexpr LengthType length() { return countOfIntegrals; }

    protected:
        // maximum count decimal digits that can be placed into IntegralType
        static constexpr IntegralType integralModulusDegree() {
            return IntegralType(extra::bitsCount<IntegralType>());
        }
        static constexpr ResultIntegralType integralModulus() {
            return std::numeric_limits<IntegralType>::max();
        }

    protected:
        static constexpr IntegralType zeroIntegral() { return IntegralType(0); }

    private:
        void checkTemplateParameters() {
            static_assert(countOfIntegrals > 0, "LongUnsigned Error: \
                Wrong count of integrals (only above zero)");
        }

	};

}
