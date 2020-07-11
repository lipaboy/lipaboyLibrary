#include "big_unsigned.h"
#include "extra_tools/extra_tools.h"

#include <vector>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <memory>

namespace lipaboy_lib {

	namespace long_numbers_space {

		// Memory management definitions have moved to the bottom of NumberlikeArray.hh.

		// The templates used by these constructors and converters are at the bottom of
		// BigUnsigned.hh.

		BigUnsigned::BigUnsigned(unsigned long  x) { initFromPrimitive(x); }
		BigUnsigned::BigUnsigned(unsigned int   x) { initFromPrimitive(x); }
		BigUnsigned::BigUnsigned(unsigned short x) { initFromPrimitive(x); }
		BigUnsigned::BigUnsigned(long  x) { initFromSignedPrimitive(x); }
		BigUnsigned::BigUnsigned(int   x) { initFromSignedPrimitive(x); }
		BigUnsigned::BigUnsigned(short x) { initFromSignedPrimitive(x); }

		unsigned long  BigUnsigned::toUnsignedLong() const { return convertToPrimitive      <unsigned long >(); }
		unsigned int   BigUnsigned::toUnsignedInt() const { return convertToPrimitive      <unsigned int  >(); }
		unsigned short BigUnsigned::toUnsignedShort() const { return convertToPrimitive      <unsigned short>(); }
		long           BigUnsigned::toLong() const { return convertToSignedPrimitive<         long >(); }
		int            BigUnsigned::toInt() const { return convertToSignedPrimitive<         int  >(); }
		short          BigUnsigned::toShort() const { return convertToSignedPrimitive<         short>(); }

		// BIT/BLOCK ACCESSORS

		void BigUnsigned::setBlock(IndexType i, BlockType newBlock) {
			if (newBlock == 0) {
				if (i < length()) {
					blocks_[i] = 0;
					zapLeadingZeros();
				}
			}
			else {
				if (i >= length()) {
					// The nonzero block extends the number.
					resize(i + 1, 0);
					// Zero any added blocks that we aren't setting.
					/*for (IndexType j = old size; j < i; j++)
						blocks[j] = 0;*/
				}
				blocks_[i] = newBlock;
			}
		}

		void BigUnsigned::zapLeadingZeros() {
			int len = blocks_.size();
			while (len > 0 && blocks_[len - 1] == 0)
				len--;
			if (len == 0)
				setToZero();
			else
				resize(len);
		}

		/* Evidently the compiler wants BigUnsigned:: on the return type because, at
		 * that point, it hasn't yet parsed the BigUnsigned:: on the name to get the
		 * proper scope. */
		BigUnsigned::IndexType BigUnsigned::bitLength() const {
			if (isZero())
				return 0;
			else {
				BlockType leftmostBlock = getBlockDirty(length() - 1);
				IndexType leftmostBlockLen = 0;
				while (leftmostBlock != 0) {
					leftmostBlock >>= 1;
					leftmostBlockLen++;
				}
				return leftmostBlockLen + (length() - 1) * BITS_PER_BLOCK;
			}
		}

		void BigUnsigned::setBit(IndexType bi, bool newBit) {
			IndexType blockI = bi / BITS_PER_BLOCK;
			BlockType block = (blockI < length()) ? getBlockDirty(blockI) : 0;
			BlockType mask = BlockType(1) << (bi % BITS_PER_BLOCK);
			block = newBit ? (block | mask) : (block & ~mask);
			setBlock(blockI, block);
		}

		// COMPARISON
		BigUnsigned::CmpRes BigUnsigned::compareTo(const BigUnsigned &x) const {
			// A bigger getLength()gth implies a bigger number.
			if (length() < x.length())
				return less;
			else if (length() > x.length())
				return greater;
			else {
				// Compare blocks one by one from left to right.
				IndexType i = length();
				while (i > 0) {
					i--;
					if (blocks_[i] == x.blocks_[i])
						continue;
					else if (blocks_[i] > x.blocks_[i])
						return greater;
					else
						return less;
				}
				// If no blocks differed, the numbers are equal.
				return equal;
			}
		}

		// COPY-LESS OPERATIONS

		/*
		 * On most calls to copy-less operations, it's safe to read the inputs little by
		 * little and write the outputs little by little.  However, if one of the
		 * inputs is coming from the same variable into which the output is to be
		 * stored (an "aliased" call), we risk overwriting the input before we read it.
		 * In this case, we first compute the result into a temporary BigUnsigned
		 * variable and then copy it into the requested output variable *this.
		 * Each put-here operation uses the DTRT_ALIASED macro (Do The Right Thing on
		 * aliased calls) to generate code for this check.
		 *
		 * I adopted this approach on 2007.02.13 (see Assignment Operators in
		 * BigUnsigned.hh).  Before then, put-here operations rejected aliased calls
		 * with an exception.  I think doing the right thing is better.
		 *
		 * Some of the put-here operations can probably handle aliased calls safely
		 * without the extra copy because (for example) they process blocks strictly
		 * right-to-left.  At some point I might determine which ones don't need the
		 * copy, but my reasoning would need to be verified very carefully.  For now
		 * I'll leave in the copy.
		 */

		using BlockType = typename BigUnsignedView::BlockType;
		using IndexType = typename BigUnsigned::IndexType;

		BigUnsigned const & BigUnsigned::add(BigUnsignedView first, BigUnsignedView second) {
			// TODO: add check on sum with yourself
			if (&this->blocks_ == &(first.container()) || &this->blocks_ == &(second.container())) {
				BigUnsigned temp;
				temp.add(first, second);
				return (*this = temp);
			}

			bool carryIn, carryOut;
			BlockType temp;
			IndexType i;
			// Set prelimiary getLength()gth and make room in this BigUnsigned
			BigUnsignedView longNum = (second.length() > first.length()) ? second : first;
			BigUnsignedView shortNum = (second.length() > first.length()) ? first : second;
			
			resize(longNum.length() + 1);

			// For each block index that is present in both inputs...
			for (i = 0, carryIn = false; i < shortNum.length(); i++) {
				// Add input blocks
				temp = longNum.getBlockDirty(i) + shortNum.getBlockDirty(i);
				// If a rollover occurred, the result is less than either input.
				// This test is used many times in the BigUnsigned code.
				carryOut = (temp < longNum.getBlockDirty(i));
				// If a carry was input, handle it
				if (carryIn) {
					temp++;
					carryOut |= (temp == 0);
				}
				blocks_[i] = temp; // Save the addition result
				carryIn = carryOut; // Pass the carry along
			}
			// If there is a carry left over, increase blocks until
			// one does not roll over.
			for (; i < longNum.length() && carryIn; i++) {
				temp = longNum.getBlockDirty(i) + 1;
				carryIn = (temp == 0);
				blocks_[i] = temp;
			}
			// If the carry was resolved but the larger number
			// still has blocks, copy them over.
			for (; i < longNum.length(); i++)
				blocks_[i] = longNum.getBlockDirty(i);
			// Set the extra block if there's still a carry, decrease getLength()gth otherwise
			if (carryIn)
				blocks_[i] = 1;
			else
				resize(length() - 1);

			return *this;
		}

		BigUnsigned operator +(BigUnsignedView first, BigUnsignedView second) {
			BigUnsigned result;
			result.add(first, second);
			return result;
		}

		// THINK ABOUT: really necessary to do check for negative result?
		BigUnsigned operator -(BigUnsignedView first, BigUnsignedView second) {
			BigUnsigned result;

			if (first.length() < second.length())
				// If a is shorter than b, the result is negative.
				throw std::logic_error("BigUnsigned::subtract: "
					"Negative result in unsigned calculation");
			// Some variables...
			bool borrowIn, borrowOut;
			BlockType temp;
			IndexType i;
			// Set preliminary getLength()gth and make room
			result.resize(first.length());
			// For each block index that is present in both inputs...
			for (i = 0, borrowIn = false; i < second.length(); i++) {
				temp = first.getBlockDirty(i) - second.getBlockDirty(i);
				// If a reverse rollover occurred,
				// the result is greater than the block from a.
				borrowOut = (temp > first.getBlockDirty(i));
				// Handle an incoming borrow
				if (borrowIn) {
					borrowOut |= (temp == 0);
					temp--;
				}
				result.blocks_[i] = temp; // Save the subtraction result
				borrowIn = borrowOut; // Pass the borrow along
			}
			// If there is a borrow left over, decrease blocks until
			// one does not reverse rollover.
			for (; i < first.length() && borrowIn; i++) {
				borrowIn = (first.getBlockDirty(i) == 0);
				result.blocks_[i] = first.getBlockDirty(i) - 1;
			}
			/* If there's still a borrow, the result is negative.
			 * Throw an exception, but zero out this object so as to leave it in a
			 * predictable state. */
			if (borrowIn) {
				result.setToZero();
				throw "BigUnsigned::subtract: Negative result in unsigned calculation";
			}
			else
				// Copy over the rest of the blocks
				for (; i < first.length(); i++)
					result.blocks_[i] = first.getBlockDirty(i);
			// Zap leading zeros
			result.zapLeadingZeros();

			return result;
		}

		/*
		 * About the multiplication and division algorithms:
		 *
		 * I searched unsucessfully for fast C++ built-in operations like the `b_0'
		 * and `c_0' Knuth describes in Section 4.3.1 of ``The Art of Computer
		 * Programming'' (replace `place' by `Blk'):
		 *
		 *    ``b_0[:] multiplication of a one-place integer by another one-place
		 *      integer, giving a two-place answer;
		 *
		 *    ``c_0[:] division of a two-place integer by a one-place integer,
		 *      provided that the quotient is a one-place integer, and yielding
		 *      also a one-place remainder.''
		 *
		 * I also missed his note that ``[b]y adjusting the word size, if
		 * necessary, nearly all computers will have these three operations
		 * available'', so I gave up on trying to use algorithms similar to his.
		 * A future version of the library might include such algorithms; I
		 * would welcome contributions from others for this.
		 *
		 * I eventually decided to use bit-shifting algorithms.  To multiply `a'
		 * and `b', we zero out the result.  Then, for each `1' bit in `a', we
		 * shift `b' left the appropriate amount and add it to the result.
		 * Similarly, to divide `a' by `b', we shift `b' left varying amounts,
		 * repeatedly trying to subtract it from `a'.  When we succeed, we note
		 * the fact by setting a bit in the quotient.  While these algorithms
		 * have the same O(n^2) time complexity as Knuth's, the ``constant factor''
		 * is likely to be larger.
		 *
		 * Because I used these algorithms, which require single-block addition
		 * and subtraction rather than single-block multiplication and division,
		 * the innermost loops of all four routines are very similar.  Study one
		 * of them and all will become clear.
		 */

		 /*
		  * This is a little inline function used by both the multiplication
		  * routine and the division routine.
		  *
		  * `getShiftedBlock' returns the `x'th block of `num << y'.
		  * `y' may be anything from 0 to N - 1, and `x' may be anything from
		  * 0 to `num.getLength()'.
		  *
		  * Two things contribute to this block:
		  *
		  * (1) The `N - y' low bits of `num.blk[x]', shifted `y' bits left.
		  *
		  * (2) The `y' high bits of `num.blk[x-1]', shifted `N - y' bits right.
		  *
		  * But we must be careful if `x == 0' or `x == num.getLength()', in
		  * which case we should use 0 instead of (2) or (1), respectively.
		  *
		  * If `y == 0', then (2) contributes 0, as it should.  However,
		  * in some computer environments, for a reason I cannot understand,
		  * `a >> b' means `a >> (b % N)'.  This means `num.blk[x-1] >> (N - y)'
		  * will return `num.blk[x-1]' instead of the desired 0 when `y == 0';
		  * the test `y == 0' handles this case specially.
		  */
		inline BigUnsigned::BlockType getShiftedBlock(const BigUnsigned &num,
			BigUnsigned::IndexType x, unsigned int y) 
		{
			BigUnsigned::BlockType part1 = (x == 0 || y == 0) ? 0 
				: (num.blocks_[x - 1] >> (BigUnsigned::BITS_PER_BLOCK - y));
			BigUnsigned::BlockType part2 = (x == num.length()) ? 0 : (num.blocks_[x] << y);
			return part1 | part2;
		}

		/*namespace {
			using ContainerType = typename BigUnsigned::BlockContainer;

			ContainerType multiplyByKaracuba2(ContainerType const & first, ContainerType const & second)
			{

			}
		}*/

		using std::shared_ptr;
		using std::vector;
		using NumberTypePointer = shared_ptr<BigUnsigned>;
		using StorageType = vector<NumberTypePointer>;

		static StorageType storage;
		static size_t storageEndPos = 0;

		NumberTypePointer allocFromStorage(StorageType & storage, size_t & storageEndPos) {
			if (storageEndPos >= storage.size()) {
				storage.push_back(std::make_shared<BigUnsigned>());
				storageEndPos++;
				return storage.back();
			}
			// Else
			storageEndPos++;
			return storage[storageEndPos - 1];
		}

		void deallocFromStorage(size_t & storageEndPos) {
			storageEndPos--;
		}

		NumberTypePointer multiplyByKaracuba2(BigUnsignedView first, BigUnsignedView second, long long& a) {
			using IntegralType = typename BigUnsigned::BlockType;
			using DoubleType = unsigned long long int;

			NumberTypePointer result = allocFromStorage(storage, storageEndPos);

            //int init = storageEndPos;

			if (first.length() == 1 && second.length() == 1) {\
				DoubleType mult = static_cast<DoubleType>(first.getBlockDirty(0)) * second.getBlockDirty(0);
				result->resize(2);
				result->blocks_[0] = static_cast<IntegralType>(
					mult & setBitsFromStart<DoubleType>(sizeof(IntegralType) * 8));
				result->blocks_[1] = static_cast<IntegralType>((mult >> (sizeof(IntegralType) * 8))
					& setBitsFromStart<DoubleType>(sizeof(IntegralType) * 8));
				result->zapLeadingZeros();
			}
			else {
				BigUnsignedView longNumber = (first.length() >= second.length()) ? first : second;
				BigUnsignedView shortNumber = (first.length() < second.length()) ? first : second;
				auto halfSize = longNumber.length() / 2;

				if (shortNumber.length() > halfSize) {
					BigUnsignedView minorLong(longNumber.begin(), std::next(longNumber.begin(), halfSize), 
						longNumber.container());
					BigUnsignedView minorShort(shortNumber.begin(),
						shortNumber.length() > halfSize ? std::next(shortNumber.begin(), halfSize)
						: shortNumber.end(), shortNumber.container());

					// Recursive descent
					NumberTypePointer minorMult = multiplyByKaracuba2(minorLong, minorShort, a);

					BigUnsignedView majorLong(std::next(longNumber.begin(), halfSize), longNumber.end(), 
						longNumber.container());
					BigUnsignedView majorShort(std::next(shortNumber.begin(), halfSize), shortNumber.end(), 
						shortNumber.container());

					// Recursive descent
					NumberTypePointer majorMult = multiplyByKaracuba2(majorLong, majorShort, a);

					NumberTypePointer sumLong = allocFromStorage(storage, storageEndPos);
					sumLong->add(majorLong, minorLong);
					NumberTypePointer sumShort = allocFromStorage(storage, storageEndPos);
					sumShort->add(majorShort, minorShort);
					// Recursive descent
					NumberTypePointer multOfSums = multiplyByKaracuba2(*sumLong, *sumShort, a);
					NumberTypePointer differ = allocFromStorage(storage, storageEndPos);
					*differ = (*multOfSums - *majorMult - *minorMult);

					auto differShiftSize = sizeof(IntegralType) * 8 * halfSize;
					//*result = *minorMult + (*differ << (differShiftSize)) + (*majorMult << (2 * differShiftSize));
					result->add(*minorMult, (*differ << (differShiftSize)));
					result->add(*result, (*majorMult << (2 * differShiftSize)));

					//minor mult
					deallocFromStorage(storageEndPos);
					//major mult
					deallocFromStorage(storageEndPos);
					//mult of sums
					deallocFromStorage(storageEndPos);
					//differ
					deallocFromStorage(storageEndPos);
					//sumLong
					deallocFromStorage(storageEndPos);
					//sumShort
					deallocFromStorage(storageEndPos);
				}
				else {
					BigUnsignedView minorLong(longNumber.begin(), std::next(longNumber.begin(), halfSize), 
						longNumber.container());
					BigUnsignedView minorShort(shortNumber.begin(),
						shortNumber.length() > halfSize ? std::next(shortNumber.begin(), halfSize)
						: shortNumber.end(), shortNumber.container());

					// Recursive descent
					NumberTypePointer minorMult = multiplyByKaracuba2(minorLong, minorShort, a);

					BigUnsignedView majorLong(std::next(longNumber.begin(), halfSize), longNumber.end(), 
						longNumber.container());

					NumberTypePointer sumLong = allocFromStorage(storage, storageEndPos);
					sumLong->add(majorLong, minorLong);
					// Recursive descent
					NumberTypePointer multOfSums = multiplyByKaracuba2(*sumLong, minorShort, a);
					NumberTypePointer differ = allocFromStorage(storage, storageEndPos);
					*differ = (*multOfSums - *minorMult);

					auto differShiftSize = sizeof(IntegralType) * 8 * halfSize;
					result->add(*minorMult, (*differ << (differShiftSize)));

					//minor mult
					deallocFromStorage(storageEndPos);
					//differ
					deallocFromStorage(storageEndPos);
					//mult of sums
					deallocFromStorage(storageEndPos);
					//sumLong
					deallocFromStorage(storageEndPos);
				}

			}

			a = std::max<long long>(a, storageEndPos);

			return result;
		}

		BigUnsigned multiplyByKaracuba(BigUnsignedView first, BigUnsignedView second) {
			long long a = 0;
			reallocCount = 0;
			auto res = *multiplyByKaracuba2(first, second, a);
			deallocFromStorage(storageEndPos);
			cout << "Realloc count: " << reallocCount << endl;
			return res;
		}

		// TODO: Replace this macros on smth else
#define DTRT_ALIASED(cond, op) \
	if (cond) { \
		BigUnsigned tmpThis; \
		tmpThis.op; \
		*this = tmpThis; \
		return; \
	}

		void BigUnsigned::multiply(const BigUnsigned &a, const BigUnsigned &b) {
			DTRT_ALIASED(this == &a || this == &b, multiply(a, b));
			// If either a or b is zero, set to zero.
			if (a.isZero() || b.isZero()) {
				setToZero();
				return;
			}
			/*
			 * Overall method:
			 *
			 * Set this = 0.
			 * For each 1-bit of `a' (say the `i2'th bit of block `i'):
			 *    Add `b << (i blocks and i2 bits)' to *this.
			 */
			 // Variables for the calculation
			IndexType i, j, k;
			unsigned int i2;
			BlockType temp;
			bool carryIn, carryOut;
			// Set preliminary getLength()gth and make room
			resize(a.length() + b.length());
			// Zero out this object
			for (i = 0; i < length(); i++)
				blocks_[i] = 0;
			// For each block of the first number...
			for (i = 0; i < a.length(); i++) {
				// For each 1-bit of that block...
				for (i2 = 0; i2 < BITS_PER_BLOCK; i2++) {
					if ((a.blocks_[i] & (BlockType(1) << i2)) == 0)
						continue;
					/*
					 * Add b to this, shifted left i blocks and i2 bits.
					 * j is the index in b, and k = i + j is the index in this.
					 *
					 * `getShiftedBlock', a short inline function defined above,
					 * is now used for the bit handling.  It replaces the more
					 * complex `bHigh' code, in which each run of the loop dealt
					 * immediately with the low bits and saved the high bits to
					 * be picked up next time.  The last run of the loop used to
					 * leave leftover high bits, which were handled separately.
					 * Instead, this loop runs an additional time with j == b.getLength().
					 * These changes were made on 2005.01.11.
					 */
					for (j = 0, k = i, carryIn = false; j <= b.length(); j++, k++) {
						/*
						 * The body of this loop is very similar to the body of the first loop
						 * in `add', except that this loop does a `+=' instead of a `+'.
						 */
						temp = blocks_[k] + getShiftedBlock(b, j, i2);
						carryOut = (temp < blocks_[k]);
						if (carryIn) {
							temp++;
							carryOut |= (temp == 0);
						}
						blocks_[k] = temp;
						carryIn = carryOut;
					}
					// No more extra iteration to deal with `bHigh'.
					// Roll-over a carry as necessary.
					for (; carryIn; k++) {
						blocks_[k]++;
						carryIn = (blocks_[k] == 0);
					}
				}
			}
			// Zap possible leading zero
			zapLeadingZeros();
		}

		/*
		 * DIVISION WITH REMAINDER
		 * This monstrous function mods *this by the given divisor b while storing the
		 * quotient in the given object q; at the end, *this contains the remainder.
		 * The seemingly bizarre pattern of inputs and outputs was chosen so that the
		 * function copies as little as possible (since it is implemented by repeated
		 * subtraction of multiples of b from *this).
		 *
		 * "modWithQuotient" might be a better name for this function, but I would
		 * rather not change the name now.
		 */
		void BigUnsigned::divideWithRemainder(const BigUnsigned &b, BigUnsigned &q) {
			/* Defending against aliased calls is more complex than usual because we
			 * are writing to both *this and q.
			 *
			 * It would be silly to try to write quotient and remainder to the
			 * same variable.  Rule that out right away. */
			if (this == &q)
				throw "BigUnsigned::divideWithRemainder: Cannot write quotient and remainder into the same variable";
			/* Now *this and q are separate, so the only concern is that b might be
			 * aliased to one of them.  If so, use a temporary copy of b. */
			if (this == &b || &q == &b) {
				BigUnsigned tmpB(b);
				divideWithRemainder(tmpB, q);
				return;
			}

			/*
			 * Knuth's definition of mod (which this function uses) is somewhat
			 * different from the C++ definition of % in case of division by 0.
			 *
			 * We let a / 0 == 0 (it doesn't matter much) and a % 0 == a, no
			 * exceptions thrown.  This allows us to preserve both Knuth's demand
			 * that a mod 0 == a and the useful property that
			 * (a / b) * b + (a % b) == a.
			 */
			if (b.isZero()) {
				q.setToZero();
				return;
			}

			/*
			 * If *this.getLength() < b.getLength(), then *this < b, and we can be sure that b doesn't go into
			 * *this at all.  The quotient is 0 and *this is already the remainder (so leave it alone).
			 */
			if (length() < b.length()) {
				q.setToZero();
				return;
			}

			// At this point we know (*this).getLength() >= b.getLength() > 0.  (Whew!)

			/*
			 * Overall method:
			 *
			 * For each appropriate i and i2, decreasing:
			 *    Subtract (b << (i blocks and i2 bits)) from *this, storing the
			 *      result in subtractBuf.
			 *    If the subtraction succeeds with a nonnegative result:
			 *        Turn on bit i2 of block i of the quotient q.
			 *        Copy subtractBuf back into *this.
			 *    Otherwise bit i2 of block i remains off, and *this is unchanged.
			 *
			 * Eventually q will contain the entire quotient, and *this will
			 * be left with the remainder.
			 *
			 * subtractBuf[x] corresponds to blk[x], not blk[x+i], since 2005.01.11.
			 * But on a single iteration, we don't touch the i lowest blocks of blk
			 * (and don't use those of subtractBuf) because these blocks are
			 * unaffected by the subtraction: we are subtracting
			 * (b << (i blocks and i2 bits)), which ends in at least `i' zero
			 * blocks. */
			 // Variables for the calculation
			IndexType i, j, k;
			unsigned int i2;
			BlockType temp;
			bool borrowIn, borrowOut;

			/*
			 * Make sure we have an extra zero block just past the value.
			 *
			 * When we attempt a subtraction, we might shift `b' so
			 * its first block begins a few bits left of the dividend,
			 * and then we'll try to compare these extra bits with
			 * a nonexistent block to the left of the dividend.  The
			 * extra zero block ensures sensible behavior; we need
			 * an extra block in `subtractBuf' for exactly the same reason.
			 */
			IndexType origLen = length(); // Save real getLength()gth.
			/* To avoid an out-of-bounds access in case of reallocation, allocate
			 * first and then increment the logical getLength()gth. */
			resize(length() + 1);
			blocks_[origLen] = 0; // Zero the added block.

			// subtractBuf holds part of the result of a subtraction; see above.
			BlockType *subtractBuf = new BlockType[length()];

			// Set preliminary getLength()gth for quotient and make room
			q.resize(origLen - b.length() + 1);
			// Zero out the quotient
			for (i = 0; i < q.length(); i++)
				q.blocks_[i] = 0;

			// For each possible left-shift of b in blocks...
			i = q.length();
			while (i > 0) {
				i--;
				// For each possible left-shift of b in bits...
				// (Remember, N is the number of bits in a Blk.)
				q.blocks_[i] = 0;
				i2 = BITS_PER_BLOCK;
				while (i2 > 0) {
					i2--;
					/*
					 * Subtract b, shifted left i blocks and i2 bits, from *this,
					 * and store the answer in subtractBuf.  In the for loop, `k == i + j'.
					 *
					 * Compare this to the middle section of `multiply'.  They
					 * are in many ways analogous.  See especially the discussion
					 * of `getShiftedBlock'.
					 */
					for (j = 0, k = i, borrowIn = false; j <= b.length(); j++, k++) {
						temp = blocks_[k] - getShiftedBlock(b, j, i2);
						borrowOut = (temp > blocks_[k]);
						if (borrowIn) {
							borrowOut |= (temp == 0);
							temp--;
						}
						// Since 2005.01.11, indices of `subtractBuf' directly match those of `blk', so use `k'.
						subtractBuf[k] = temp;
						borrowIn = borrowOut;
					}
					// No more extra iteration to deal with `bHigh'.
					// Roll-over a borrow as necessary.
					for (; k < origLen && borrowIn; k++) {
						borrowIn = (blocks_[k] == 0);
						subtractBuf[k] = blocks_[k] - 1;
					}
					/*
					 * If the subtraction was performed successfully (!borrowIn),
					 * set bit i2 in block i of the quotient.
					 *
					 * Then, copy the portion of subtractBuf filled by the subtraction
					 * back to *this.  This portion starts with block i and ends--
					 * where?  Not necessarily at block `i + b.getLength()'!  Well, we
					 * increased k every time we saved a block into subtractBuf, so
					 * the region of subtractBuf we copy is just [i, k).
					 */
					if (!borrowIn) {
						q.blocks_[i] |= (BlockType(1) << i2);
						while (k > i) {
							k--;
							blocks_[k] = subtractBuf[k];
						}
					}
				}
			}
			// Zap possible leading zero in quotient
			q.zapLeadingZeros();
			// Zap any/all leading zeros in remainder
			zapLeadingZeros();
			// Deallocate subtractBuf.
			// (Thanks to Brad Spencer for noticing my accidental omission of this!)
			delete[] subtractBuf;
		}

		/* BITWISE OPERATORS
		 * These are straightforward blockwise operations except that they differ in
		 * the output getLength()gth and the necessity of zapLeadingZeros. */

		void BigUnsigned::bitAnd(const BigUnsigned &a, const BigUnsigned &b) {
			DTRT_ALIASED(this == &a || this == &b, bitAnd(a, b));
			// The bitwise & can't be longer than either operand.
			resize((a.length() >= b.length()) ? b.length() : a.length());
			IndexType i;
			for (i = 0; i < length(); i++)
				blocks_[i] = a.blocks_[i] & b.blocks_[i];
			zapLeadingZeros();
		}

		void BigUnsigned::bitOr(const BigUnsigned &a, const BigUnsigned &b) {
			DTRT_ALIASED(this == &a || this == &b, bitOr(a, b));
			IndexType i;
			const BigUnsigned *a2, *b2;
			if (a.length() >= b.length()) {
				a2 = &a;
				b2 = &b;
			}
			else {
				a2 = &b;
				b2 = &a;
			}
			resize(a2->length());
			for (i = 0; i < b2->length(); i++)
				blocks_[i] = a2->blocks_[i] | b2->blocks_[i];
			for (; i < a2->length(); i++)
				blocks_[i] = a2->blocks_[i];
			resize(a2->length());
			// Doesn't need zapLeadingZeros.
		}

		void BigUnsigned::bitXor(const BigUnsigned &a, const BigUnsigned &b) {
			DTRT_ALIASED(this == &a || this == &b, bitXor(a, b));
			IndexType i;
			const BigUnsigned *a2, *b2;
			if (a.length() >= b.length()) {
				a2 = &a;
				b2 = &b;
			}
			else {
				a2 = &b;
				b2 = &a;
			}
			resize(a2->length());
			for (i = 0; i < b2->length(); i++)
				blocks_[i] = a2->blocks_[i] ^ b2->blocks_[i];
			for (; i < a2->length(); i++)
				blocks_[i] = a2->blocks_[i];
			resize(a2->length());
			zapLeadingZeros();
		}

		void BigUnsigned::bitShiftLeft(const BigUnsigned &a, int b) {
			DTRT_ALIASED(this == &a, bitShiftLeft(a, b));
			if (b < 0) {
				if (b << 1 == 0)
					throw "BigUnsigned::bitShiftLeft: "
					"Pathological shift amount not implemented";
				else {
					bitShiftRight(a, -b);
					return;
				}
			}
			IndexType shiftBlocks = b / BITS_PER_BLOCK;
			unsigned int shiftBits = b % BITS_PER_BLOCK;
			// + 1: room for high bits nudged left into another block
			resize(a.length() + shiftBlocks + 1);
			IndexType i, j;
			for (i = 0; i < shiftBlocks; i++)
				blocks_[i] = 0;
			for (j = 0, i = shiftBlocks; j <= a.length(); j++, i++)
				blocks_[i] = getShiftedBlock(a, j, shiftBits);
			// Zap possible leading zero
			zapLeadingZeros();
		}

		void BigUnsigned::bitShiftRight(const BigUnsigned &a, int b) {
			DTRT_ALIASED(this == &a, bitShiftRight(a, b));
			if (b < 0) {
				if (b << 1 == 0)
					throw "BigUnsigned::bitShiftRight: "
					"Pathological shift amount not implemented";
				else {
					bitShiftLeft(a, -b);
					return;
				}
			}
			// This calculation is wacky, but expressing the shift as a left bit shift
			// within each block lets us use getShiftedBlock.
			IndexType rightShiftBlocks = (b + BITS_PER_BLOCK - 1) / BITS_PER_BLOCK;
			unsigned int leftShiftBits = BITS_PER_BLOCK * rightShiftBlocks - b;
			// Now (N * rightShiftBlocks - leftShiftBits) == b
			// and 0 <= leftShiftBits < N.
			if (rightShiftBlocks >= a.length() + 1) {
				// All of a is guaranteed to be shifted off, even considering the left
				// bit shift.
				setToZero();
				return;
			}
			// Now we're allocating a positive amount.
			// + 1: room for high bits nudged left into another block
			resize(a.length() + 1 - rightShiftBlocks);
			IndexType i, j;
			for (j = rightShiftBlocks, i = 0; j <= a.length(); j++, i++)
				blocks_[i] = getShiftedBlock(a, j, leftShiftBits);
			// Zap possible leading zero
			zapLeadingZeros();
		}

		// INCREMENT/DECREMENT OPERATORS

		// Prefix increment
		void BigUnsigned::operator ++() {
			IndexType i;
			bool carry = true;
			for (i = 0; i < length() && carry; i++) {
				blocks_[i]++;
				carry = (blocks_[i] == 0);
			}
			if (carry) {
				// Allocate and then increase getLength()gth, as in divideWithRemainder
				resize(length() + 1);
				blocks_[i] = 1;
			}
		}

		// Postfix increment: same as prefix
		void BigUnsigned::operator ++(int) {
			operator ++();
		}

		// Prefix decrement
		void BigUnsigned::operator --() {
			if (isZero())
				throw "BigUnsigned::operator --(): Cannot decrement an unsigned zero";
			IndexType i;
			bool borrow = true;
			for (i = 0; borrow; i++) {
				borrow = (blocks_[i] == 0);
				blocks_[i]--;
			}
			// Zap possible leading zero (there can only be one)
			zapLeadingZeros();
		}

		// Postfix decrement: same as prefix
		void BigUnsigned::operator --(int) {
			operator --();
		}

	}

}
