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


int kek (int a) { return a * a; }
bool kekBool (int a) { return a % 2; }

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

//TEST(Get, Finite_N) {
//    auto res = createStream(1, 2, 5, 6, 1900, 234)
//            | get(4)
//            | to_vector();

//    ASSERT_THAT(res, ElementsAreArray({ 1, 2, 5, 6 }));
//}

//TEST(Get, Finite_Overflow) {
//    auto res = createStream(1, 2, 5, 6, 1900, 234)
//            | get(100)
//            | to_vector();

//    ASSERT_THAT(res, ElementsAreArray({ 1, 2, 5, 6, 1900, 234 }));
//}

TEST(Get, InfiniteStream) {
    int a = 1;
    cout << "\tStart" << endl;
    auto res = createStream([&a]() { return a++; })
            | filter([] (int b) -> bool { return b % 2 != 0; })
            | filter([] (int b) -> bool { return b % 3 != 0; })
            | get(11)
            | to_vector();
cout << "\tEnd" << endl;
    ASSERT_EQ(res, vector<int>({ 1, 5, 7, 11 }));
}

TEST(Get, Infinite_Empty) {
    int a = 0;
    auto res = createStream([&a]() { return a++; })
            | get(0)
            | to_vector();

    ASSERT_TRUE(res.empty());
}

TEST(StreamTest, check) {
    try {
//        auto ss = Stream({1, 2});



//        vector<int> olala = { 1, 2, 3, 4, 5, 6, 7, 8 };
//        auto stream2 = createStream(olala.begin(), olala.end());
//        (stream2 | print_to(cout)) << endl;
//        auto stream3 = stream2 | map(kek);
//        auto stream4 = stream2 | filter([] (int a) -> bool { return a % 2 == 0; }) | map(kek)
//                | filter([] (int a) -> bool { return a % 10 == 6; });

//        (stream3 | print_to(cout)) << endl;
//        cout << "Start filter: " << endl;
//        (stream4 | print_to(cout)) << endl;
//        cout << "End filter: " << endl;

//        list<int> list1 = { 8, 10, 13 };
//        auto stream5 = createStream(list1.begin(), list1.end());
//        (stream5  | get(2) | print_to(cout)) << endl;

//        // Generator
//        auto stream6 = createStream([] () { return rand() % 228; });

//        auto stream8 = stream6 | map(kek) | get(20) | filter(kekBool);
//        (stream8 | get(20) | print_to(cout)) << endl;
//        ASSERT_NO_THROW(stream6 | map(kek) | filter(kekBool) | get(10) | print_to(cout));

//        (stream8 | print_to(cout)) << endl;
//        vector<int> elem = (stream8 | group(5) | nth(2));
//        auto elem2 = (stream8 | group(5) | group(3) | nth(0));
//        cout << typeid(elem).name() << endl;
//        cout << "{ ";
//        std::copy(elem.begin(), elem.end(), std::ostream_iterator<int>(cout, " "));
//        cout << "} ";
//        cout << endl;
//        cout << "{{ ";
//        for (int i = 0; i < 5; i++)
//            cout << elem2[0][i] << ", ";
//        cout << "}} " << endl;

//        vector<string> strs = { string("kek1"), string("kek2") };
//        auto stream7 = createStream(strs.begin(), strs.end());
//        (stream7 | map([] (string s) { return s + "o"; }) | print_to(cout)) << endl;

//        auto stream9 = createStream(1, 2, 3, 4, 5);
//        (stream9 | skip(1) | skip(1) | get(2) | print_to(cout, "-")) << endl;

//        auto lolVec = stream9 | group(1) | filter([] (const vector<int>& kek) { return kek[0] % 2 == 0; }) | nth(0);
//        cout << lolVec[0] << endl;

//        vector<int> vecRes = stream9 | map([] (int i) { return i * i * 3; }) | to_vector();
//        cout << vecRes[3] << endl;

//        cout << "Sum = " << (stream9 | reduce([] (int) { return 0; },
//            [] (int a, int b) { return a + b; })) << endl;
//        cout << "Sum = " << (stream9 | sum()) << endl;

        //-------------Noisy Test---------------//

//        vector<Noisy> vecNoisy(5);
//        auto streamNoisy = createStream(vecNoisy.begin(), vecNoisy.end());
//        Noisy el = (streamNoisy
//                    //| map([] (const Noisy& a) -> Noisy { return a; })
//                    | Get(4) | Get(4)
//                    | filter([] (const Noisy& a) { static int i = 0; return (i++ % 2 == 0); })
//                    | Get(4) | nth(0));
//        Stream<VectorIterator> stream10(5, 228, 69);
    } catch (std::bad_alloc & exp) {
        cout << exp.what() << endl;
    }
}

}
