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
using std::unordered_map;
using std::unique_ptr;
using std::tuple;

using namespace stream_space;
using namespace functors_space;

//---------------------------------Tests-------------------------------//


//-------------------------------------//


TEST(Filter, sample) {
    auto res = createStream(1, 2, 3)
            | filter([](auto x) { return (x == x); })
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 1, 2, 3 }));
}
TEST(Filter, mod3) {
    auto res = createStream(1, 2, 3, 4, 5, 6)
            | filter([](auto x) { return x % 3 == 0; })
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 3, 6 }));
}

TEST(Get, Finite_N) {
    auto res = createStream(1, 2, 5, 6, 1900, 234)
            | get(4)
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 1, 2, 5, 6 }));
}

TEST(Get, Finite_Overflow) {
    auto res = createStream(1, 2, 5, 6, 1900, 234)
            | get(100)
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 1, 2, 5, 6, 1900, 234 }));
}

TEST(Get, InfiniteStream) {
    int a = 1;
    auto res = createStream([&a]() { return a++; })
            | filter([] (int b) -> bool { return b % 2 != 0; })
            | filter([] (int b) -> bool { return b % 3 != 0; })
            | get(11)
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 1, 5, 7, 11 }));
}

TEST(Exception, Infinite) {
    int a = 1;
    auto stream = createStream([&a]() { return a++; });
    ASSERT_ANY_THROW(stream | to_vector());
    ASSERT_ANY_THROW(stream
                     | map([] (int a) { return 2 * a; })
                     | to_vector());
}

TEST(Get, Infinite_Empty) {
    int a = 0;
    auto res = createStream([&a]() { return a++; })
            | get(0)
            | to_vector();

    ASSERT_TRUE(res.empty());
}

TEST(Skip, Infinite) {
    int a = 0;
    auto res = createStream([&a]() { return a++; })
            | get(4)
            | skip(2)
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 2, 3 }));
}

TEST(Skip, Finite) {
    std::list<int> lol = { 1, 2, 3 };
    auto res = createStream(lol.begin(), lol.end())
            | skip(2)
            | to_vector();

    ASSERT_EQ(res, vector<int>({ 3 }));
}

TEST(FileStream, read) {
    std::ofstream outFile;
    string filename = "temp.stream.lol";
    outFile.open(filename, std::ios::out | std::ios::trunc);
    string fileData = "lol kek cheburek";
    outFile << fileData;
    outFile.close();

    std::ifstream inFile;
    inFile.open(filename, std::ios::in | std::ios::binary);

//    auto begin = std::istreambuf_iterator<char>(inFile);
//    cout << *(begin + 1) << *begin << endl;
    auto fileStream = createStream(std::istreambuf_iterator<char>(inFile),
                                   std::istreambuf_iterator<char>());

    auto res = fileStream
            | map([] (auto ch) { return ch + 1; })
            | map([] (auto ch) { return ch - 1; })
            | reduce([] (auto ch) { return string(1, ch); },
                     [] (string& str, auto ch) { return str + string(1, ch); });
    ASSERT_EQ(res, fileData);

    inFile.close();
    std::remove(filename.c_str());
}

TEST(Group, Infinite) {
    int a = 0;
    auto res = createStream([&a]() { return a++; })
            | get(4)
            | group(2)
            | nth(1);
    ASSERT_EQ(res, decltype(res)({ 2, 3 }));
}

TEST(Reduce, Infinite) {
    int a = 0;
    auto res = createStream([&a]() { return a++; })
            | get(4)
            | reduce([] (int res, int elem) { return res + elem; });
    ASSERT_EQ(res, 6);
}

TEST(Sum, Infinite) {
    int a = 0;
    auto res = createStream([&a]() { return a++; })
            | get(4)
            | sum();
    ASSERT_EQ(res, 6);
}

TEST(UngroupByBit, init_list) {
    cout << endl << endl;
    cout << endl << endl;

    // need method currentElem!!
    vector<char> olala = { 'a', 'b', 'c' };
    createStream(olala.begin(), olala.end())
            | ungroupByBit()
            | print_to(cout, " ");

cout << endl << endl;
cout << endl << endl;

//    ASSERT_EQ(kek, decltype(kek)({ 16, 36 }));
}

int kek (int a) { return a * a; }
bool kekBool (int a) { return a % 2; }

TEST(Filter, init_list) {
    vector<int> olala = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto stream2 = createStream(olala.begin(), olala.end());
    auto stream4 = stream2 | filter([] (int a) -> bool { return a % 2 == 0; }) | map(kek)
            | filter([] (int a) -> bool { return a % 10 == 6; });
    auto kek = stream4 | to_vector();

    ASSERT_EQ(kek, decltype(kek)({ 16, 36 }));
}

TEST(StreamTest, noisy) {
    try {
        //-------------Noisy Test---------------//

//        vector<Noisy> vecNoisy(5);
//        auto streamNoisy = createStream(vecNoisy.begin(), vecNoisy.end());
//        (streamNoisy
//                    //| map([] (const Noisy& a) -> Noisy { return a; })
//                    | get(4) | get(4)
//                    | filter([] (const Noisy& a) { static int i = 0; return (i++ % 2 == 0); })
//                    | get(4) | nth(0));
    } catch (std::bad_alloc & exp) {
        cout << exp.what() << endl;
    }
}

}
