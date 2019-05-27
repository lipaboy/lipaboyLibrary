#include "big_unsigned_in_a_base.h"

namespace lipaboy_lib {

	namespace long_numbers_space {

		BigUnsignedInABase::BigUnsignedInABase(const Digit *d, IndexType l, Base base)
			: digits(d, d + l), base(base) {
			// Check the base
			if (base < 2)
				throw "BigUnsignedInABase::BigUnsignedInABase(const Digit *, Index, Base): The base must be at least 2";

			// Validate the digits.
			for (IndexType i = 0; i < l; i++)
				if (digits[i] >= base)
					throw "BigUnsignedInABase::BigUnsignedInABase(const Digit *, Index, Base): A digit is too large for the specified base";

			// Eliminate any leading zeros we may have been passed.
			zapLeadingZeros();
		}

		namespace {
			unsigned int bitLen(unsigned int x) {
				unsigned int len = 0;
				while (x > 0) {
					x >>= 1;
					len++;
				}
				return len;
			}
			unsigned int ceilingDiv(unsigned int a, unsigned int b) {
				return (a + b - 1) / b;
			}
		}

		BigUnsignedInABase::BigUnsignedInABase(const BigUnsigned &x, Base base) : digits(1, 0) {
			// Check the base
			if (base < 2)
				throw "BigUnsignedInABase(BigUnsigned, Base): The base must be at least 2";
			this->base = base;

			// Get an upper bound on how much space we need
			int maxBitLenOfX = x.getLength() * BigUnsigned::BITS_PER_BLOCK;
			int minBitsPerDigit = bitLen(base) - 1;
			int maxDigitLenOfX = ceilingDiv(maxBitLenOfX, minBitsPerDigit);
			if (maxDigitLenOfX <= 0)
				setToZero();
			else
				digits.resize(maxDigitLenOfX); // Another change to comply with `staying in bounds'.

			BigUnsigned x2(x), buBase(base);
			IndexType digitNum = 0;

			while (!x2.isZero()) {
				// Get last digit.  This is like `lastDigit = x2 % buBase, x2 /= buBase'.
				BigUnsigned lastDigit(x2);
				lastDigit.divideWithRemainder(buBase, x2);
				// Save the digit.
				digits[digitNum] = lastDigit.toUnsignedShort();
				// Move on.  We can't run out of room: we figured it out above.
				digitNum++;
			}

			// Save the actual length.
			digits.resize(digitNum);
		}

		BigUnsignedInABase::operator BigUnsigned() const {
			BigUnsigned ans(0), buBase(base), temp;
			IndexType digitNum = getLength();
			while (digitNum > 0) {
				digitNum--;
				temp.multiply(ans, buBase);
				ans.add(temp, BigUnsigned(digits[digitNum]));
			}
			return ans;
		}

		BigUnsignedInABase::BigUnsignedInABase(const std::string &s, Base base) {
			// Check the base.
			if (base > 36)
				throw "BigUnsignedInABase(std::string, Base): The default string conversion \
					routines use the symbol set 0-9, A-Z and therefore support only up to base 36. \
					You tried a conversion with a base over 36; write your own string conversion routine.";
			// Save the base.
			// This pattern is seldom seen in C++, but the analogous ``this.'' is common in Java.
			this->base = base;

			// `s.length()' is a `size_t', while `len' is a `NumberlikeArray::Index',
			// also known as an `unsigned int'.  Some compilers warn without this cast.
			digits.resize(IndexType(s.length()));

			IndexType digitNum, symbolNumInString;
			for (digitNum = 0; digitNum < getLength(); digitNum++) {
				symbolNumInString = getLength() - 1 - digitNum;
				char theSymbol = s[symbolNumInString];
				if (theSymbol >= '0' && theSymbol <= '9')
					digits[digitNum] = theSymbol - '0';
				else if (theSymbol >= 'A' && theSymbol <= 'Z')
					digits[digitNum] = theSymbol - 'A' + 10;
				else if (theSymbol >= 'a' && theSymbol <= 'z')
					digits[digitNum] = theSymbol - 'a' + 10;
				else
					throw "BigUnsignedInABase(std::string, Base): Bad symbol in input.  Only 0-9, A-Z, a-z are accepted.";

				if (digits[digitNum] >= base)
					throw "BigUnsignedInABase::BigUnsignedInABase(const Digit *, Index, Base): \
						A digit is too large for the specified base";
			}
			zapLeadingZeros();
		}

		BigUnsignedInABase::operator std::string() const {
			if (base > 36)
				throw "BigUnsignedInABase ==> std::string: The default string \
					conversion routines use the symbol set 0-9, A-Z and therefore \
					support only up to base 36.  You tried a conversion with a base over 36; \
					write your own string conversion routine.";
			if (isZero())
				return std::string("0");
			// Some compilers don't have push_back, so use a char * buffer instead.
			char *s = new char[getLength() + 1];
			s[getLength()] = '\0';
			IndexType digitNum, symbolNumInString;
			for (symbolNumInString = 0; symbolNumInString < getLength(); symbolNumInString++) {
				digitNum = getLength() - 1 - symbolNumInString;
				Digit theDigit = digits[digitNum];
				if (theDigit < 10)
					s[symbolNumInString] = char('0' + theDigit);
				else
					s[symbolNumInString] = char('A' + theDigit - 10);
			}
			std::string s2(s);
			delete[] s;
			return s2;
		}

	}

}
