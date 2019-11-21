#include <iostream>
#include <vector>
#include <list>
#include <cstdint>

#include <iterator>
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>
#include <cstdio>

#include <fstream>

#include <functional>
#include <tuple>

#include <gtest/gtest.h>

#include "stream_test.h"

namespace stream_tests {

using std::cout;
using std::endl;
using std::vector;
using std::list;
using std::string;
using std::unique_ptr;

using namespace lipaboy_lib;

#ifdef STREAM_V1_TESTS_RUN
using namespace lipaboy_lib::stream_v1_space;
using namespace lipaboy_lib::stream_v1_space::operators_space;
#else
using namespace lipaboy_lib::stream_space;
using namespace lipaboy_lib::stream_space::operators_space;
#endif

//---------------------------------Tests-------------------------------//


//-------------------------------------//


//----------Constructor-----------//

TEST_F(PrepareStreamTest, copy_constructor) {
    auto temp = Stream(begin(), end()) | get(pOutsideContainer->size() - 1);
    auto obj = StreamBase(temp);

    ASSERT_TRUE(temp == obj);

    auto obj2 = StreamBase(Stream(begin(), end()));
    static_assert(std::is_same_v<decltype(obj2), decltype(Stream(begin(), end()))>, "lol");
    ASSERT_TRUE(obj2 == Stream(begin(), end()));
}

TEST_F(PrepareStreamTest, move_constructor) {
    auto temp = Stream(begin(), end()) | get(pOutsideContainer->size() - 1);
    auto temp2 = StreamBase(temp);
    auto obj = StreamBase(std::move(temp2));

    ASSERT_TRUE(temp == obj);

    auto temp3 = StreamBase(Stream(begin(), end()));
    auto obj2 = StreamBase(std::move(temp3));
    ASSERT_TRUE(obj2 == Stream(begin(), end()));
}

TEST_F(PrepareStreamTest, copy_constructor_by_extending_the_stream) {
    auto obj = typename StreamType::template
            ExtendedStreamType<get>(get(pOutsideContainer->size() - 1), Stream(begin(), end()));

    ASSERT_TRUE(Stream(begin(), end()) == static_cast<StreamType&>(obj));
}

TEST_F(PrepareStreamTest, move_constructor_by_extending_the_stream) {
    auto temp = StreamBase(Stream(begin(), end()));
    auto obj = typename StreamType::template
            ExtendedStreamType<get>(get(pOutsideContainer->size() - 1), std::move(temp));

    ASSERT_TRUE(Stream(begin(), end()) == static_cast<StreamType&>(obj));
}


//----------Other-----------//


TEST(Filter, sample) {
	int i = 0;
    auto res = Stream(1, 2, 3)
            | filter([] (auto x) { return (x == x); })
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 1, 2, 3 }));
}
TEST(Filter, mod3) {
    auto res = Stream(1, 2, 3, 4, 5, 6)
            | filter([](int x) { return x % 3 == 0; })
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 3, 6 }));
}

TEST(Get, Finite_N) {
    auto res = Stream(1, 2, 5, 6, 1900, 234)
            | get(4)
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 1, 2, 5, 6 }));
}

TEST(Get, Finite_Overflow) {
    auto res = Stream(1, 2, 5, 6, 1900, 234)
            | get(100)
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 1, 2, 5, 6, 1900, 234 }));
}

TEST(Get, InfiniteStream) {
    int a = 1;
    auto res = Stream([&a]() { return a++; })
            | filter([] (int b) -> bool { return b % 2 != 0; })
            | filter([] (int b) -> bool { return b % 3 != 0; })
            | get(4)
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 1, 5, 7, 11 }));
}

TEST(Infinite, check_is_so) {
    int a = 1;
    auto stream = Stream([&a]() { return a++; })
		| map([](int a) { return 2 * a; });
	ASSERT_TRUE(stream.isInfinite());
}

TEST(Get, Infinite_Empty) {
    int a = 0;
    auto res = Stream([&a]() { return a++; })
            | get(0)
            | to_vector();

    ASSERT_TRUE(res.empty());
}

TEST(Get, Group_Infinite) {
	int a = 0;
	auto res = Stream([&a]() { return a++; })
		| group_by_vector(2)
		| get(4)
		| nth(2);

	ASSERT_EQ(res, vector<int>({ 4, 5 }));
}

//----------------Get operator testing-------------------//

TEST_F(PrepareStreamTest, Get_Empty) {
    auto stream2 = Stream(begin(), end())
            | get(0);
    auto res = stream2
            | to_vector();

    ASSERT_TRUE(res.empty());
}

TEST_F(PrepareStreamTest, Get_Not_Empty) {
    auto res = Stream(begin(), end())
            | get(pOutsideContainer->size())
            | to_vector();

    ASSERT_EQ(res, *pOutsideContainer);
}

//----------------NTH operator testing-------------------//

TEST_F(PrepareStreamTest, Nth_first_elem) {
    auto res = Stream(begin(), end())
            | nth(0);
    auto res2 = Stream(begin(), end())
            | map([] (typename PrepareStreamTest::ElemType a) { return a; })
            | nth(0);

    ASSERT_EQ(res, (*pOutsideContainer)[0]);
    ASSERT_EQ(res2, (*pOutsideContainer)[0]);
}

TEST_F(PrepareStreamTest, Nth_last_elem) {
    auto res = Stream(begin(), end())
            | map([] (typename PrepareStreamTest::ElemType a) { return a; })
            | nth(pOutsideContainer->size() - 1);

    ASSERT_EQ(res, pOutsideContainer->back());
}

TEST_F(PrepareStreamTest, Nth_out_of_range) {
	ASSERT_ANY_THROW(Stream(begin(), end()) | nth(pOutsideContainer->size()));
}

TEST_F(PrepareStreamTest, Nth_out_of_range_by_negative_index) {
	ASSERT_ANY_THROW(Stream(begin(), end()) | nth(-1));
}

TEST_F(PrepareStreamTest, Nth_out_of_range_in_extended_stream) {
    ASSERT_ANY_THROW(Stream(begin(), end())
                    | map([] (typename PrepareStreamTest::ElemType a) { return a; })
                    | nth(pOutsideContainer->size()));
	//Stream(begin(), end()) | print_to(cout);
}

//----------------Skip operator testing-------------------//

TEST(Skip, Infinite) {
	int a = 0;
    auto res = Stream([&a]() { return a++; })
            | get(4)
            | skip(2)
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 2, 3 }));
}

TEST(Skip, Group_Infinite) {
	int a = 0;
	auto res = Stream([&a]() { return a++; })
		| get(4)
		| group_by_vector(2)
		| skip(1)
		| nth(0);

	ASSERT_EQ(res, vector<int>({ 2, 3 }));
}

TEST(Skip, Finite) {
    std::list<int> lol = { 1, 2, 3 };
    auto res = Stream(lol.begin(), lol.end())
            | skip(1)
            | skip(1)
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 3 }));
}

TEST_F(PrepareStreamTest, FileStream_read) {
    std::ifstream inFile;
    inFile.open(filename, std::ios::in | std::ios::binary);

    auto fileStream = Stream(std::istreambuf_iterator<char>(inFile),
                                   std::istreambuf_iterator<char>());

    auto res = fileStream
            | map([] (char ch) { return ch + 1; })
            | map([] (char ch) { return ch - 1; })
            | reduce([] (char ch) { return string(1, ch); },
                     [] (string& str, char ch) { return str + string(1, ch); });
    ASSERT_EQ(res, fileData);

    inFile.close();
}

TEST(Group, Infinite) {
    int a = 0;
    auto res = Stream([&a]() { return a++; })
            | get(4)
            | group_by_vector(2)
            | nth(1);
    ASSERT_EQ(res, decltype(res)({ 2, 3 }));
}

TEST(Reduce, Infinite) {
    int a = 0;
    auto res = Stream([&a]() { return a++; })
            | get(4)
            | reduce([] (int res, int elem) { return res + elem; });
    ASSERT_EQ(res, 6);
}

TEST(Sum, Infinite) {
    int a = 0;
    auto res = Stream([&a]() { return a++; })
            | get(4)
            | sum();
    ASSERT_EQ(res, 6);
}

TEST(UngroupByBit, init_list) {
    vector<char> olala = { 1, 2 };
    auto vecVec = Stream(olala.begin(), olala.end())
            | ungroup_by_bit()
            | group_by_vector(8)
            | to_vector();

    ASSERT_EQ(vecVec, decltype(vecVec)({ vector<bool>({1,0,0,0,0,0,0,0}),
                                         vector<bool>({0,1,0,0,0,0,0,0})})
              );
}

//-------------------------//

// BUG: bug is found here
TEST(NTH, tempValueCopying) {
	int a = 0;
	auto stream = Stream([&a]() { return a++; }) 
		| get(6);
	auto elem = stream | nth(0);
	auto stream2 = stream;
	auto elem2 = stream2 | nth(0);

	EXPECT_EQ(elem, 0);
	ASSERT_EQ(elem2, 1);
}

int sqr (int a) { return a * a; }
bool kekBool (int a) { return a % 2 == 0; }

TEST(Filter, vector_int) {
    vector<int> olala = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto stream2 = Stream(olala.begin(), olala.end());
    auto stream4 = stream2 | filter(kekBool)
            | map(sqr)
            | filter([] (int a) -> bool { return a % 10 == 6; });
    auto kek = stream4 | to_vector();

    ASSERT_EQ(kek, decltype(kek)({ 16, 36 }));
}

TEST(Filter, group_by_vector) {
    vector<int> olala = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto stream2 = Stream(olala.begin(), olala.end());
    auto kek = stream2
            | filter([] (auto a) { return a % 2 == 0; })
            | group_by_vector(3)
            | to_vector();

    ASSERT_EQ(kek, decltype(kek)({ vector<int>({ 2, 4, 6 }), vector<int>({ 8 }) }));
}

TEST(GroupByVector, filter) {
	vector<int> olala = { 1, 2, 3, 4, 5, 6, 7, 8 };
	auto stream2 = Stream(olala.begin(), olala.end());
	auto kek = stream2
		| group_by_vector(3)
        | filter([](auto const & vec) { return vec[0] % 2 == 0; })
// why I cannot use auto&
		| to_vector();

	ASSERT_EQ(kek, decltype(kek)({ vector<int>({ 4, 5, 6 }) }));
}

//-----------------Distinct----------------//

TEST(Distinct, simple) {
	vector<int> lol { 1, 1, 2, 3, 1, 1, 2, 4 };
	auto elem = Stream(lol.begin(), lol.end())
		| distinct() | to_vector();
	EXPECT_EQ(elem, decltype(elem)({ 1, 2, 3, 4 }));

	vector<string> lol2 { "lol", "kek", "lol", "kek", "kra" };
	auto elem2 = Stream(lol2.begin(), lol2.end())
		| distinct() | nth(2);
	EXPECT_EQ(elem2, "kra");

	auto stream = Stream(lol2.begin(), lol2.end())
		| distinct();
	auto stream2 = stream;
	auto elem3 = stream2 | to_vector();
	EXPECT_EQ(elem3, decltype(elem3)({ "lol", "kek", "kra" }));
}

TEST(Distinct, map_test) {
	vector<int> lol{ 1, 1, 2, 3, 1, 1, 2, 4 };
	auto elem = Stream(lol.begin(), lol.end())
		| map([](int i) { return std::to_string(i); }) 
		| map([](string str) { return std::make_unique<string>(str); })
		| map([](unique_ptr<string> p) { return *p; }) 
		| distinct()
		| to_vector();
	ASSERT_EQ(elem, decltype(elem)({ "1", "2", "3", "4" }));
}

using lipaboy_lib_tests::NoisyD;

TEST(StreamTest, unique_ptr_test) {
	using move_only = std::unique_ptr<int>;
	move_only lol[] = { std::unique_ptr<int>(new int(5)) };
	auto stream = Stream(lol) 
		| map([](auto&& elem) -> move_only { return std::move(elem); });

	ASSERT_EQ(*(stream | nth(0)), 5);

	move_only lol2[] = { std::unique_ptr<int>(new int(3)) };
	auto stream2 = Stream(lol2, lol2 + 1) 
	//	| filter([](auto& elem) { return true; })
		;
	//ASSERT_EQ(*(stream2 | nth(0)), 3);
}

TEST(StreamTest, noisy) {
    try {
        //-------------Noisy Test---------------//

#ifdef LOL_DEBUG_NOISY

        vector<NoisyD> vecNoisy(1);
        auto streamNoisy = Stream(std::move_iterator(vecNoisy.begin()), 
			std::move_iterator(vecNoisy.end()));
        cout << "\tstart streaming" << endl;
        auto streamTemp2 =
                (streamNoisy
                    //| map([] (NoisyD&& a) -> NoisyD { return std::move(a); })
                    //| map([] (NoisyD&& a) -> NoisyD { return std::move(a); })
                    //| map([] (NoisyD&& a) -> NoisyD { return std::move(a); })
					//---
					| map([](NoisyD& a) -> NoisyD& { int b = 0; return a; })
					| map([](NoisyD& a) -> NoisyD& { int b = 0; return a; })
					//---
    //                    | get(4)
    //                    | get(4)
    //                    | filter([] (const Noisy& a) { static int i = 0; return (i++ % 2 == 0); })
    //                    | get(4)
    //                | nth(0)
                );

        cout << "\tend streaming" << endl;
        if (!vecNoisy.empty())
            streamTemp2 | nth(0);
        cout << "\tend nth" << endl;
        cout << "\t------------" << endl;

        cout << "\tstart streaming" << endl;
		auto streamTemp =
			(Stream(vecNoisy.begin(), vecNoisy.end())
                | filter([] (const NoisyD& ) { return true; })
                //| filter([] (const NoisyD& ) { return true; })
                //| filter([] (const NoisyD& ) { return true; })
//                    | get(4)
//                    | get(4)
//                    | filter([] (const Noisy& a) { static int i = 0; return (i++ % 2 == 0); })
//                    | get(4)
//                | nth(0)
            );
        cout << "\tend streaming" << endl;
        if (!vecNoisy.empty())
            streamTemp | nth(0);
        cout << "\tend nth" << endl;

#endif
    } catch (std::bad_alloc & exp) {
        cout << exp.what() << endl;
    }
}

}

