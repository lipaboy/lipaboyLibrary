#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>

#include "extra_tools/extra_tools.h"
#include "intervals/cutoffborders.h"

namespace lipaboy_lib {

namespace long_numbers_space {

	// Motivation: I wanna check the theorem in Theory of Numbers 
	//			   (you know it or you can find it in pale red pocket book "Theory of Numbers in Cryptography")

// PLAN
// ----
// TODO: write constructor which will be parse the std::string of number (Deserialization).
// TODO: write the method to serialization the number into std::string.

using std::array;
using std::string;
using lipaboy_lib::cutOffLeftBorder;
using lipaboy_lib::enable_if_else_t;

// Concept: it is simple long number, without any trivial optimizations like
//			checking if number is increasing or not (in order to making less computations)
//			and without move-semantics
// Hint: don't use std::vector for a while

////////////////////////////////////////////////////////////////////////////////////////

namespace extra {

	template <class TWord>
	inline constexpr TWord setBitsFromStart(size_t bitsCount) {
		return (bitsCount <= 0) ? TWord(0) 
			: (bitsCount <= 1) ? TWord(1) 
			: (1 << (bitsCount - 1)) | setBitsFromStart(bitsCount - 1);
	}

	template <class TWord>
	inline constexpr size_t bitsCount() { return sizeof(TWord) * 8; }

}

// Requirements: 
// 1) TIntegral and TResult must be unsigned.

template <size_t lengthOfIntegrals>     // count of integral type variables
class LongIntegerDecimal
{
public:
	using TIntegral = std::uint32_t;
	using TResult = std::uint64_t;
    using IntegralType =
        std::remove_reference_t<
            enable_if_else_t<2 * sizeof(TIntegral) == sizeof(TResult), TIntegral, void> >;
    using ResultType = std::remove_reference_t<TResult>;
	using ContainerType = array<IntegralType, lengthOfIntegrals>;
	using iterator = typename ContainerType::iterator;

public:
	// Note: Non-initialized constructor: without filling array by zero value.
	explicit
	LongIntegerDecimal() {
		checkTemplateParameters();
	}
    explicit
    LongIntegerDecimal(int32_t small) {
		checkTemplateParameters();
        number_[0] = small;
		std::fill(std::next(begin()), end(), TIntegral(0));
    }
	LongIntegerDecimal(LongIntegerDecimal const & other) {
		checkTemplateParameters();
		std::copy(other.begin(), other.end(), this->begin());
	}

	LongIntegerDecimal(string const & numberDecimalStr) {
		constexpr IntegralType decimalModulus = extra::bitsCount<IntegralType>() / 3 - 1;

		int end = numberDecimalStr.size();
		int begin = cutOffLeftBorder<int>(end - decimalModulus, 0);
		for (size_t i = 0; end - begin > 0; i++) {
			// for optimization you need to see the StringView (foly library)
			auto sub = numberDecimalStr.substr(begin, end - begin);
			IntegralType part = static_cast<IntegralType>(std::stoi(sub));
			number_[i] = part;

			end -= decimalModulus;
			begin = cutOffLeftBorder<int>(begin - decimalModulus, 0);
		}
	}

	LongIntegerDecimal operator+(LongIntegerDecimal const & other) {
		LongIntegerDecimal res(*this);
		constexpr TResult lessHalfOfBits = extra::setBitsFromStart<TResult>(extra::bitsCount<TWord>() / 2);
		constexpr TResult moreHalfOfBits = ~lessHalfOfBits;

		// Think_About: maybe std::partial_sum can be useful?

		IntegralType lessPart = IntegralType(0);
		IntegralType morePart = IntegralType(0);
		for (size_t i = 0; i < length(); i++) {
			TResult doubleTemp = res[i] + other[i] + morePart;

			lessPart = doubleTemp & lessHalfOfBits;
			morePart = doubleTemp & moreHalfOfBits;

			res[i] = lessPart;
		}

		return res;
	}

	//------------Setters, Getters----------//

	constexpr size_t length() { return lengthOfIntegrals; }

protected:
	iterator begin() { return number_.begin(); }
	iterator end() { return number_.end(); }

private:
	void checkTemplateParameters() {
		static_assert(lengthOfIntegrals > 0, "Wrong length of LongInteger");
	}

private:
    array<IntegralType, lengthOfIntegrals> number_;
	bool sign_;
};

}

}
