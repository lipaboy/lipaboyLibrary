#pragma once


#include "big_unsigned.h"
#include <string>
#include <vector>

namespace lipaboy_lib {

	namespace long_numbers_space {

		/*
		 * A BigUnsignedInABase object represents a nonnegative integer of size limited
		 * only by available memory, represented in a user-specified base that can fit
		 * in an `unsigned short' (most can, and this saves memory).
		 *
		 * BigUnsignedInABase is intended as an intermediary class with little
		 * functionality of its own.  BigUnsignedInABase objects can be constructed
		 * from, and converted to, BigUnsigneds (requiring multiplication, mods, etc.)
		 * and `std::string's (by switching digit values for appropriate characters).
		 *
		 * BigUnsignedInABase is similar to BigUnsigned.  Note the following:
		 *
		 * (1) They represent the number in exactly the same way, except that
		 * BigUnsignedInABase uses ``digits'' (or Digit) where BigUnsigned uses
		 * ``blocks'' (or Blk).
		 *
		 * (2) Both use the management features of NumberlikeArray.  (In fact, my desire
		 * to add a BigUnsignedInABase class without duplicating a lot of code led me to
		 * introduce NumberlikeArray.)
		 *
		 * (3) The only arithmetic operation supported by BigUnsignedInABase is an
		 * equality test.  Use BigUnsigned for arithmetic.
		 */

		using std::vector;

		class BigUnsignedInABase
		{
		public:
			// The digits of a BigUnsignedInABase are unsigned shorts.
			using Digit = typename BigUnsigned::BlockType;
			using IndexType = typename BigUnsigned::IndexType;
			// That's also the type of a base.
			using Base = Digit;

			using size_type = size_t;

		private:
			vector<Digit> digits;

		protected:
			// The base in which this BigUnsignedInABase is expressed
			Base base;

			// Creates a BigUnsignedInABase with a capacity; for internal use.
			BigUnsignedInABase(int, IndexType c) : digits(1, c) {}

			// Decreases len to eliminate any leading zero digits.
			void zapLeadingZeros() {
				int len = getLength();
				while (len > 0 && digits[len - 1] == 0)
					len--;
				digits.resize(len > 0 ? len : 1);
			}

		public:
			// Constructs zero in base 2.
			BigUnsignedInABase() : digits(1, 0), base(2) {}

			// Copy constructor
			BigUnsignedInABase(const BigUnsignedInABase &x) : digits(x.digits), base(x.base) {}

			// Assignment operator
			void operator =(const BigUnsignedInABase &x) {
				digits = x.digits;
				base = x.base;
			}

			// Constructor that copies from a given array of digits.
			BigUnsignedInABase(const Digit *d, IndexType l, Base base);

			// Destructor.  NumberlikeArray does the delete for us.
			~BigUnsignedInABase() {}

			// LINKS TO BIGUNSIGNED
			BigUnsignedInABase(const BigUnsigned &x, Base base);
			operator BigUnsigned() const;

			/* LINKS TO STRINGS
			 *
			 * These use the symbols ``0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'' to
			 * represent digits of 0 through 35.  When parsing strings, lowercase is
			 * also accepted.
			 *
			 * All string representations are big-endian (big-place-value digits
			 * first).  (Computer scientists have adopted zero-based counting; why
			 * can't they tolerate little-endian numbers?)
			 *
			 * No string representation has a ``base indicator'' like ``0x''.
			 *
			 * An exception is made for zero: it is converted to ``0'' and not the
			 * empty string.
			 *
			 * If you want different conventions, write your own routines to go
			 * between BigUnsignedInABase and strings.  It's not hard.
			 */
			operator std::string() const;
			BigUnsignedInABase(const std::string &s, Base base);

		public:

			// ACCESSORS
			Base getBase() const { return base; }

			// Expose these from NumberlikeArray directly.
			size_type getCapacity() const { return digits.capacity(); }
			size_type getLength() const { return digits.size(); }

			/* Returns the requested digit, or 0 if it is beyond the length (as if
			 * the number had 0s infinitely to the left). */
			Digit getDigit(IndexType i) const { return digits[i]; }

			// The number is zero if and only if the canonical length is zero.
			bool isZero() const { return getLength() <= 1 && digits[0] == 0; }

			void setToZero() {
				digits.resize(1);
				digits[0] = 0;
			}

			/* Equality test.  For the purposes of this test, two BigUnsignedInABase
			 * values must have the same base to be equal. */
			bool operator ==(const BigUnsignedInABase &other) const {
				// Definitely unequal.
				if (getLength() != other.getLength() || base != other.base)
					return false;

				// Compare corresponding blocks one by one.
				for (IndexType i = 0; i < getLength(); i++)
					if (digits[i] != other.digits[i])
						return false;
				// No blocks differed, so the objects are equal.
				return true;
			}
			bool operator !=(const BigUnsignedInABase &x) const { return !operator ==(x); }

		};

	}

}
