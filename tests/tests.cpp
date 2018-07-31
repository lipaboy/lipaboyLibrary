#include "gtest/gtest.h"

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "common_interfaces/algebra.h"
#include "maths/fixed_precision_number.h"
#include "intervals/interval.h"

#include "stream/stream_test.h"
#include "extra_tools/extra_tools_tests.h"

namespace lipaboy_lib_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unique_ptr;

using lipaboy_lib::FixedPrecisionNumber;
using lipaboy_lib::Interval;


TEST(Algebra, check) {
    FixedPrecisionNumber<double, int, 1, -5>
            kek1(2),
            kek2(3);
    FixedPrecisionNumber<double, int, 1, -8>
            kuk(-4);

    ASSERT_EQ(kek1 + kek2, 5);
    ASSERT_EQ(kek1 + 4., 6.);
    ASSERT_EQ((kek1 + kek2) / (kek1 - kek2), -5);
    ASSERT_EQ(kek1 + kuk, -2);

    ASSERT_EQ(kek1 - kek2, -1.);
    ASSERT_EQ(kek1 - 4., -2.);
    ASSERT_EQ(kek1 - kuk, 6.);

    ASSERT_EQ(kek1 * kek2, 6.);
    ASSERT_EQ(kek1 * 4., 8.);
    ASSERT_EQ(kek1 * kuk, -8.);

    ASSERT_EQ(kek1 / kek2, 2. / 3.);
    ASSERT_EQ(kek1 / 4.,   1. / 2.);
    ASSERT_EQ(kek1 / kuk, -1. / 2.);
}

TEST(EitherComparable, comparison) {
    FixedPrecisionNumber<double, int, 1, -5>
            kek1(2);
    // not work
//    ASSERT_NE(kek1, 2.00002);
    ASSERT_EQ(kek1, 2.00001);
}

TEST(Interval, contains) {
    Interval<int, std::less<>, std::less<> > interval(1, 5);
    ASSERT_TRUE(interval.containsAll(2, 3, 4));
    ASSERT_TRUE(interval.containsNone(0, 1, 5));
}


class PtrDeallocationWrapper {
public:
    virtual ~PtrDeallocationWrapper() {}
};

template <class T>
class PtrDeallocationWrapperOnHeap
        : public PtrDeallocationWrapper
{
public:
    template <class... Args>
    PtrDeallocationWrapperOnHeap(Args&&... args)
        : ptr_(new T(std::forward<Args>(args)...))
    {}
    virtual ~PtrDeallocationWrapperOnHeap() {
        cout << "heap deallocation" << endl;
    }

private:
    unique_ptr<T> ptr_;
};

template <class T>
class PtrDeallocationWrapperOnStack
        : public PtrDeallocationWrapper
{
public:
    PtrDeallocationWrapperOnStack(T * ptr) : ptr_(ptr) {}
    virtual ~PtrDeallocationWrapperOnStack() {
        cout << "stack not deallocation" << endl;
    }

private:
    T * ptr_;
};

template <class T, class... Args>
auto make_heap_ptr(Args&&... args)
    -> unique_ptr<PtrDeallocationWrapper>
{
    return std::move(unique_ptr<PtrDeallocationWrapper>(
                         new PtrDeallocationWrapperOnHeap<T>(std::forward<Args>(args)...))
                     );
}

template <class T>
auto make_stack_ptr(T* ptr)
    -> unique_ptr<PtrDeallocationWrapper>
{
    return std::move(unique_ptr<PtrDeallocationWrapper>(
                         new PtrDeallocationWrapperOnStack<T>(ptr))
                     );
}

TEST(Check, check) {
    vector<unique_ptr<PtrDeallocationWrapper> > vec;

    vec.push_back(make_heap_ptr<string>("lol"));
    string str1 = "kek";
    vec.push_back(make_stack_ptr<string>(&str1));

    vec.pop_back();
    vec.pop_back();
}

}



int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
	auto res = RUN_ALL_TESTS();
#ifdef WIN32
	system("pause");
#endif
    return res;
}
