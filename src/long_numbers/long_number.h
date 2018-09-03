#pragma once

#include <functional>
#include <iterator>
#include <numeric>
#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <cmath>

#include "extra_tools/extra_tools.h"
#include "intervals/cutoffborders.h"
#include "extra_tools/maths_tools.h"

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
using lipaboy_lib::powDozen;

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
			: (TWord(1) << (bitsCount - 1)) | setBitsFromStart<TWord>(bitsCount - 1);
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
	using const_iterator = typename ContainerType::const_iterator;

	using reference = IntegralType&;
	using const_reference = const IntegralType&;

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
		std::copy(other.cbegin(), other.cend(), this->begin());
	}

	LongIntegerDecimal(string const & numberDecimalStr) {
		int last = numberDecimalStr.size();
		int first = cutOffLeftBorder<int>(last - modulusDegree(), 0);
		size_t i = 0;
		for (; last - first > 0; i++) {
			// for optimization you need to see the StringView (foly library)
			auto sub = numberDecimalStr.substr(first, last - first);
			IntegralType part = static_cast<IntegralType>(std::stoi(sub));
			number_[i] = part;

			last -= modulusDegree();
			first = cutOffLeftBorder<int>(first - modulusDegree(), 0);
		}
		std::fill(std::next(begin(), i), end(), zero());
	}

	LongIntegerDecimal operator+(LongIntegerDecimal const & other) const {
		LongIntegerDecimal res(*this);
		constexpr size_t halfBits = extra::bitsCount<TResult>() / 2;
		constexpr TResult lessHalfOfBits = extra::setBitsFromStart<TResult>(halfBits);
		constexpr TResult moreHalfOfBits = ~lessHalfOfBits;

		// Think_About: maybe std::partial_sum can be useful?

		IntegralType lessPart = zero();
		IntegralType morePart = zero();
		for (size_t i = 0; i < length(); i++) {
			TResult doubleTemp = res[i] + other[i] + morePart;

			lessPart = doubleTemp & lessHalfOfBits;
			morePart = static_cast<IntegralType>((doubleTemp & moreHalfOfBits) >> halfBits);

			res[i] = lessPart % modulus();
			morePart += lessPart / modulus();
		}

		return res;
	}

	string to_string() const {
		// Opinion: bad readible
		/*return std::accumulate(std::next(cbegin()), cend(),
			std::to_string(number_[0]),
			[](string& acc, IntegralType elem) {
				return (elem == zero()) ? acc 
					: std::to_string(elem) + acc;
			});*/
		// Better realization (reallocation memory criteria)
		string res = "";
		bool isFirstNonZeroMet = false;
		for (int i = length() - 1; i >= 0; i--) {
			string part = std::to_string(number_[i]);
			if (number_[i] != zero()) {
				res += ((false == isFirstNonZeroMet) ? "" 
					: string(modulusDegree() - part.size(), '0')) 
					+ part;
				isFirstNonZeroMet = true;
			}
		}
		return res;
	}

	//------------Setters, Getters----------//

	constexpr size_t length() const { return lengthOfIntegrals; }

protected:
	constexpr IntegralType modulusDegree() const { 
		return static_cast<IntegralType>(std::floor(
			std::log(2) / std::log(10) * double(extra::bitsCount<IntegralType>()))); 
	}
	constexpr IntegralType modulus() const { return powDozen<IntegralType>(modulusDegree()); }
	constexpr IntegralType zero() const { return IntegralType(0); }

	iterator begin() { return number_.begin(); }
	iterator end() { return number_.end(); }
	const_iterator cbegin() const { return number_.cbegin(); }
	const_iterator cend() const { return number_.cend(); }
	const_reference operator[] (size_t index) const { return number_[index]; }
	reference operator[] (size_t index) { return number_[index]; }

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
