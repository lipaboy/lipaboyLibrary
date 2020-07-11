#include <gtest/gtest.h>

#include "long_numbers/big_integer_library.h"

#include <string>
#include <iostream>
#include <limits>


namespace big_integer_tests {

	using std::cout;
	using std::endl;
	using std::numeric_limits;
	using std::to_string;
	using std::string;

	using namespace lipaboy_lib::long_numbers_space;

	TEST(BigUnsigned, constructor) {
		ASSERT_TRUE(BigUnsigned().isZero());
	}

	TEST(BigInteger, constructor) {
		ASSERT_TRUE(BigInteger().isZero());
	}

	TEST(BigUnsigned, simple) {
		BigUnsigned z(0), one(1), ten(10);
		ASSERT_EQ(bigUnsignedToString(z), "0");
		ASSERT_EQ(bigUnsignedToString(one), "1");
		ASSERT_EQ(bigUnsignedToString(ten), "10");
	}

	TEST(BigUnsigned, Blk_check) {
		typename BigUnsigned::BlockType myBlocks[3];
		myBlocks[0] = 3;
		myBlocks[1] = 4;
		myBlocks[2] = 0;
		BigUnsigned bu(myBlocks, 3);
		ASSERT_EQ(bigUnsignedToString(bu), "17179869187");
		ASSERT_EQ(bigIntegerToString(BigInteger(myBlocks, 3)), "17179869187");
		ASSERT_EQ(bigIntegerToString(BigInteger(bu)), "17179869187");

		ASSERT_EQ(bigIntegerToString(BigInteger(myBlocks, 3, BigInteger::positive)), "17179869187"); //17179869187
		ASSERT_EQ(bigIntegerToString(BigInteger(myBlocks, 3, BigInteger::negative)), "-17179869187"); //-17179869187
		ASSERT_ANY_THROW(BigInteger(myBlocks, 3, BigInteger::zero)); //error
		ASSERT_EQ(bigIntegerToString(BigInteger(bu, BigInteger::positive)),"17179869187"); //17179869187
		ASSERT_EQ(bigIntegerToString(BigInteger(bu, BigInteger::negative)), "-17179869187"); //-17179869187
		ASSERT_ANY_THROW((BigInteger(bu, BigInteger::zero))); //error
	}

	TEST(BigInteger, forcing_sign_to_zero_without_error) {
		typename BigUnsigned::BlockType myZeroBlocks[1];
		myZeroBlocks[0] = 0;

		ASSERT_EQ(bigIntegerToString(BigInteger(myZeroBlocks, 1, BigInteger::positive)), "0"); //0
		ASSERT_EQ(bigIntegerToString(BigInteger(myZeroBlocks, 1, BigInteger::negative)), "0"); //0
		ASSERT_EQ(bigIntegerToString(BigInteger(myZeroBlocks, 1, BigInteger::zero)), "0"); //0
	}

	TEST(BigUnsigned, conversion_limits) {
		ASSERT_EQ(BigUnsigned(0).toUnsignedLong(), 0); //0
		ASSERT_EQ(BigUnsigned(numeric_limits<unsigned long>::max()).toUnsignedLong(), 
			numeric_limits<unsigned long>::max()); //
		ASSERT_ANY_THROW(stringToBigUnsigned(
			std::to_string(2ULL + (unsigned long long)numeric_limits<unsigned long>::max())).toUnsignedLong()); //error

		ASSERT_EQ(BigUnsigned(0).toLong(), 0); //0
		ASSERT_EQ(BigUnsigned(numeric_limits<long>::max()).toLong(), numeric_limits<long>::max()); //
        ASSERT_ANY_THROW(BigUnsigned(2u + numeric_limits<long>::max()).toLong()); //error

		// int is the same as long on a 32-bit system

		ASSERT_EQ(BigUnsigned(0).toUnsignedInt(), 0); //0
		ASSERT_EQ(BigUnsigned(numeric_limits<unsigned int>::max()).toUnsignedInt(), 
			numeric_limits<unsigned int>::max()); //
		ASSERT_ANY_THROW(stringToBigUnsigned(
			std::to_string(2ULL + (unsigned long long)numeric_limits<unsigned int>::max())).toUnsignedInt()); //error

		ASSERT_EQ(BigUnsigned(0).toInt(), 0); //0
		ASSERT_EQ(BigUnsigned(numeric_limits<int>::max()).toInt(), numeric_limits<int>::max()); //
		ASSERT_ANY_THROW(BigUnsigned(2u + numeric_limits<int>::max()).toInt()); //error

		ASSERT_EQ(BigUnsigned(0).toUnsignedShort(), 0); //0
		ASSERT_EQ(BigUnsigned(numeric_limits<unsigned short>::max()).toUnsignedShort(), numeric_limits<unsigned short>::max()); //65535
		ASSERT_ANY_THROW(BigUnsigned(2u + numeric_limits<unsigned short>::max()).toUnsignedShort()); //error

		ASSERT_EQ(BigUnsigned(0).toShort(), 0); //0
		ASSERT_EQ(BigUnsigned(numeric_limits<short>::max()).toShort(), numeric_limits<short>::max()); //32767
		ASSERT_ANY_THROW(BigUnsigned(2u + numeric_limits<short>::max()).toShort()); //error
	}

	TEST(BigInteger, conversion_limits) {
		ASSERT_ANY_THROW(BigInteger(-1).toUnsignedLong()); //error
		ASSERT_EQ(BigInteger(0).toUnsignedLong(), 0); //0
		ASSERT_EQ(BigInteger(numeric_limits<unsigned long>::max()).toUnsignedLong(),
			numeric_limits<unsigned long>::max()); //
		ASSERT_ANY_THROW(stringToBigInteger(
			std::to_string(2ULL + (unsigned long long)numeric_limits<unsigned long>::max())).toUnsignedLong()); //error

		ASSERT_ANY_THROW(stringToBigInteger(
			std::to_string(-2LL + (long long)numeric_limits<long>::min())).toLong()); //error
		ASSERT_EQ(stringToBigInteger(std::to_string(numeric_limits<long>::min())).toLong(), 
			numeric_limits<long>::min());
		ASSERT_EQ(BigInteger(numeric_limits<long>::min()).toLong(), numeric_limits<long>::min());
		ASSERT_EQ(BigInteger(0).toLong(), 0); //0
		ASSERT_EQ(BigInteger(numeric_limits<long>::max()).toLong(), numeric_limits<long>::max()); //
		ASSERT_ANY_THROW(BigInteger(2u + numeric_limits<long>::max()).toLong()); //error

		// int is the same as long on a 32-bit system

		ASSERT_ANY_THROW(BigInteger(-1).toUnsignedInt()); //error
		ASSERT_EQ(BigInteger(0).toUnsignedInt(), 0); //0
		ASSERT_EQ(BigInteger(numeric_limits<unsigned int>::max()).toUnsignedInt(),
			numeric_limits<unsigned int>::max()); //
		ASSERT_ANY_THROW(stringToBigInteger(
			std::to_string(2ULL + (unsigned long long)numeric_limits<unsigned int>::max())).toUnsignedInt()); //error

		ASSERT_ANY_THROW(stringToBigInteger(
			std::to_string(-2LL + (long long)numeric_limits<int>::min())).toInt()); //error
		ASSERT_EQ(stringToBigInteger(std::to_string(numeric_limits<int>::min())).toInt(),
			numeric_limits<int>::min());
		ASSERT_EQ(BigInteger(numeric_limits<int>::min()).toInt(), numeric_limits<int>::min());
		ASSERT_EQ(BigInteger(0).toInt(), 0); //0
		ASSERT_EQ(BigInteger(numeric_limits<int>::max()).toInt(), numeric_limits<int>::max()); //
		ASSERT_ANY_THROW(BigInteger(2u + numeric_limits<int>::max()).toInt()); //error

		ASSERT_ANY_THROW(BigInteger(-1).toUnsignedShort()); //error
		ASSERT_EQ(BigInteger(0).toUnsignedShort(), 0); //0
		ASSERT_EQ(BigInteger(numeric_limits<unsigned short>::max()).toUnsignedShort(), 
			numeric_limits<unsigned short>::max()); //65535
		ASSERT_ANY_THROW(BigInteger(2u + numeric_limits<unsigned short>::max()).toUnsignedShort()); //error

		ASSERT_EQ(BigInteger(numeric_limits<short>::min()).toShort(), numeric_limits<short>::min());
		ASSERT_EQ(BigInteger(numeric_limits<short>::min() + 1).toShort(), numeric_limits<short>::min() + 1);
		ASSERT_ANY_THROW(BigInteger(numeric_limits<short>::min() - 1).toShort());
		ASSERT_EQ(BigInteger(0).toShort(), 0); //0
		ASSERT_EQ(BigInteger(numeric_limits<short>::max()).toShort(), numeric_limits<short>::max()); //32767
		ASSERT_ANY_THROW(BigInteger(2u + numeric_limits<short>::max()).toShort()); //error
	}

	constexpr short pathologicalShort = ~((unsigned short)(~0) >> 1);
	constexpr int pathologicalInt = ~((unsigned int)(~0) >> 1);
	constexpr long pathologicalLong = ~((unsigned long)(~0) >> 1);

	TEST(BigUnsigned, conversion_limits_negative) {
		//	// ...during construction
		ASSERT_ANY_THROW(BigUnsigned(short(-1))); //error
		ASSERT_ANY_THROW(BigUnsigned(static_cast<short>(pathologicalShort))); //error
		ASSERT_ANY_THROW(BigUnsigned(-1)); //error
		ASSERT_ANY_THROW(BigUnsigned(static_cast<int>(pathologicalInt))); //error
		ASSERT_ANY_THROW(BigUnsigned(long(-1))); //error
		ASSERT_ANY_THROW(BigUnsigned(static_cast<long>(pathologicalLong))); //error

		// ...during subtraction
		ASSERT_ANY_THROW(BigUnsigned(5) - BigUnsigned(6)); //error
		ASSERT_ANY_THROW(stringToBigUnsigned("314159265358979323") - stringToBigUnsigned("314159265358979324")); //error
		ASSERT_EQ(bigUnsignedToString(BigUnsigned(5) - BigUnsigned(5)), "0"); //0
		ASSERT_EQ(bigUnsignedToString(stringToBigUnsigned("314159265358979323") 
			- stringToBigUnsigned("314159265358979323")), "0"); //0
		ASSERT_EQ(bigUnsignedToString(stringToBigUnsigned("4294967296") - BigUnsigned(1)), "4294967295"); //4294967295
	}

	TEST(BigUnsigned, addition) {
		ASSERT_EQ(bigUnsignedToString(BigUnsigned(0) + 0), "0"); //0
		ASSERT_EQ(bigUnsignedToString(BigUnsigned(0) + 1), "1"); //1
		// Ordinary carry
		ASSERT_EQ(bigUnsignedToString(stringToBigUnsigned("8589934591" /* 2^33 - 1*/)
			+ stringToBigUnsigned("4294967298" /* 2^32 + 2 */)), "12884901889");
		// Creation of a new block
		ASSERT_EQ(bigUnsignedToString(BigUnsigned(0xFFFFFFFFU) + 1), "4294967296"); //4294967296
	}

	TEST(BigUnsigned, subtraction) {
		ASSERT_EQ(bigUnsignedToString(BigUnsigned(1) - 0), "1"); //1
		ASSERT_EQ(bigUnsignedToString(BigUnsigned(1) - 1), "0"); //0
		ASSERT_EQ(bigUnsignedToString(BigUnsigned(2) - 1), "1"); //1
		// Ordinary borrow
		ASSERT_EQ(bigUnsignedToString(stringToBigUnsigned("12884901889")
			- stringToBigUnsigned("4294967298")), "8589934591"); //8589934591
		// Borrow that removes a block
		ASSERT_EQ(bigUnsignedToString(stringToBigUnsigned("4294967296") - 1), "4294967295"); //4294967295
	}

	TEST(BigUnsigned, multiplication_division) {
		BigUnsigned a = BigUnsigned(314159265) * 358979323;
		ASSERT_EQ(bigUnsignedToString(a), "112776680263877595");
		ASSERT_EQ(bigUnsignedToString(a / 123), "916883579381118");
		ASSERT_EQ(bigUnsignedToString(a % 123), "81"); //81

		ASSERT_ANY_THROW(BigUnsigned(5) / 0); //error
	}

	TEST(BigUnsigned, block_accessors) {
		BigUnsigned b;
		ASSERT_EQ(bigUnsignedToString(b), "0"); //0
		ASSERT_EQ(b.getBlock(0), 0); //0
		b.setBlock(1, 314);
		// Did b grow properly?  And did we zero intermediate blocks?
		ASSERT_EQ(bigUnsignedToString(b), "1348619730944"); //
		ASSERT_EQ(b.length(), 2); //2
		ASSERT_EQ(b.getBlock(0), 0); //0
		ASSERT_EQ(b.getBlock(1), 314); //314
		// Did b shrink properly?
		b.setBlock(1, 0);
		ASSERT_EQ(bigUnsignedToString(b), "0"); //0

		BigUnsigned bb(314);
		bb.setBlock(1, 159);
		// Make sure we used allocateAndCopy, not allocate
		ASSERT_EQ(bb.getBlock(0), 314); //314
		ASSERT_EQ(bb.getBlock(1), 159); //159
		// Blocks beyond the number should be zero regardless of whether they are
		// within the capacity.
		bb = 1 + 2;
		ASSERT_EQ(bb.getBlock(0), 3); //3
		ASSERT_EQ(bb.length(), 1);
	}

	TEST(BigUnsigned, bit_accessors) {
		ASSERT_EQ(BigUnsigned(0).bitLength(), 0); //0
		ASSERT_EQ(BigUnsigned(1).bitLength(), 1); //1
		ASSERT_EQ(BigUnsigned(4095).bitLength(), 12); //12
		ASSERT_EQ(BigUnsigned(4096).bitLength(), 13); //13
		// 5 billion is between 2^32 (about 4 billion) and 2^33 (about 8 billion).
		ASSERT_EQ(stringToBigUnsigned("5000000000").bitLength(), 33); //33

		// 25 is binary 11001.
		BigUnsigned bbb(25);
		ASSERT_EQ(bbb.getBit(4), 1); //1
		ASSERT_EQ(bbb.getBit(3), 1); //1
		ASSERT_EQ(bbb.getBit(2), 0); //0
		ASSERT_EQ(bbb.getBit(1), 0); //0
		ASSERT_EQ(bbb.getBit(0), 1); //1
		ASSERT_EQ(bbb.bitLength(), 5); //5
		// Effectively add 2^32.
		bbb.setBit(32, true);
		ASSERT_EQ(bigUnsignedToString(bbb), "4294967321");
		bbb.setBit(31, true);
		bbb.setBit(32, false);
		ASSERT_EQ(bigUnsignedToString(bbb), "2147483673");
	}

	TEST(BigNumbers, combining_different_types) {
		// === Combining BigUnsigned, BigInteger, and primitive integers ===

		BigUnsigned p1 = BigUnsigned(3) * 5;
		ASSERT_EQ(p1, 15); //15
		/* In this case, we would like g++ to implicitly promote the BigUnsigned to a
			* BigInteger, but it seems to prefer converting the -5 to a BigUnsigned, which
			* causes an error.  If I take out constructors for BigUnsigned from signed
			* primitive integers, the BigUnsigned(3) becomes ambiguous, and if I take out
			* all the constructors but BigUnsigned(unsigned long), g++ uses that
			* constructor and gets a wrong (positive) answer.  Thus, I think we'll just
			* have to live with this cast. */
		BigInteger p2 = BigInteger(BigUnsigned(3)) * -5;
		ASSERT_EQ(p2, -15); //-15
	}

	//	// === Test some previous bugs === // //

	//TEST(BigInteger, set_sign_to_zero) {
	//	{
	//		/* Test that BigInteger division sets the sign to zero.
	//			* Bug reported by David Allen. */
	//		BigInteger num(3), denom(5), quotient;
	//		num.divideWithRemainder(denom, quotient);
	//		ASSERT_EQ(quotient, 0);
	//		num = 5;
	//		num.divideWithRemainder(denom, quotient);
	//		ASSERT_EQ(num, 0);
	//	}
	//} 

	TEST(BigUnsigned, multiplication_by_karacuba) {
		BigUnsigned first(int(1e8)), second(int(1e9)), res;

		res = multiplyByKaracuba(first, second);

        EXPECT_EQ(bigUnsignedToString(res), to_string((long long)(1e17)));

        first = stringToBigUnsigned(to_string((long long)(1e12)));
        second = stringToBigUnsigned(to_string((long long)(1e5)));
		res = multiplyByKaracuba(first, second);
        EXPECT_EQ(bigUnsignedToString(res), to_string((long long)(1e17)));

        first = stringToBigUnsigned(to_string((long long)(1e12)));
		res = multiplyByKaracuba(first, first);

		EXPECT_EQ(bigUnsignedToString(first), "1000000000000");
		EXPECT_EQ(bigUnsignedToString(res), "1000000000000000000000000");

		first = stringToBigUnsigned("1234567890123");
		res = multiplyByKaracuba(first, first);

		EXPECT_EQ(bigUnsignedToString(res), "1524157875322755800955129");

	}



	int main2() {

		try {

		//	{
		//		/* Test that BigInteger division sets the sign to zero.
		//		 * Bug reported by David Allen. */
		//		BigInteger num(3), denom(5), quotient;
		//		num.divideWithRemainder(denom, quotient);
		//		check(quotient);
		//		num = 5;
		//		num.divideWithRemainder(denom, quotient);
		//		check(num);
		//	}

		//	{
		//		/* Test that BigInteger subtraction sets the sign properly.
		//		 * Bug reported by Samuel Larkin. */
		//		BigInteger zero(0), three(3), ans;
		//		ans = zero - three;
		//		TEST(check(ans).getSign()); //-1
		//	}

		//	{
		//		/* Test that BigInteger multiplication shifts bits properly on systems
		//		 * where long is bigger than int.  (Obviously, this would only catch the
		//		 * bug when run on such a system.)
		//		 * Bug reported by Mohand Mezmaz. */
		//		BigInteger f = 4; f *= 3;
		//		TEST(check(f)); //12
		//	}

		//	{
		//		/* Test that bitwise XOR allocates the larger length.
		//		 * Bug reported by Sriram Sankararaman. */
		//		BigUnsigned a(0), b(3), ans;
		//		ans = a ^ b;
		//		TEST(ans); //3
		//	}

		//	{
		//		/* Test that an aliased multiplication works.
		//		 * Bug reported by Boris Dessy. */
		//		BigInteger num(5);
		//		num *= num;
		//		TEST(check(num)); //25
		//	}

		//	{
		//		/* Test that BigUnsignedInABase(std::string) constructor rejects digits
		//		 * too big for the specified base.
		//		 * Bug reported by Niakam Kazemi. */
		//		TEST(BigUnsignedInABase("f", 10)); //error
		//	}

		}
		catch (const char *err) {
			cout << "UNCAUGHT ERROR: " << err << endl;
		}

		system("pause");

		return 0;
	}

}
