#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <string>

#include "common_interfaces/algebra.h"
#include "maths/fixed_precision_number.h"

#include "stream_test.h"

namespace check_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;

using LipaboyLib::FixedPrecisionNumber;

template <class T, class Derived_>
struct Base {
    using Derived = Derived_;
    T lol() const {
        return static_cast<Derived_ const *>(this)->lol2();
    }
};

template <class T, class Base>
struct Oper1 {
    using Derived = typename Base::Derived;
    T lol3() const {
        cout << static_cast<Derived const *>(this)->lol() << endl;
        cout << "lol" << endl;
        return T(5);
    }
};

template <class T, class Base>
struct Oper2 {
    using Derived = typename Base::Derived;
    T lol3() const {
        static_cast<Derived const *>(this)->lol();
        cout << "lol 2" << endl;
        return 5.2;
    }
};

template <class T>
struct DoubleDerived
        : Base<T, DoubleDerived<T> >,
          Oper1<T, Base<T, DoubleDerived<T> > >,
          Oper2<T, Base<T, DoubleDerived<T> > >
{
    using Oper1<T, Base<T, DoubleDerived<T> > >::lol3;
//    using Base<T, DoubleDerived<T> >::lol;

    T lol2() const {
        return T(228);
    }
};


TEST(Check, check) {
    DoubleDerived<int> dd3;
    dd3.lol3();

    FixedPrecisionNumber<double, int, 1, -5>
            kek1(2),
            kek2(3);
    FixedPrecisionNumber<double, int, 1, -8>
            kuk(-4);
    ASSERT_EQ(kek1 + kek2, 5);
    ASSERT_EQ(kek1 + 4, 6);
    ASSERT_EQ((kek1 + kek2) / (kek1 - kek2), -5);
    ASSERT_EQ(kek1 + kuk, -2);
//    ASSERT_NE(kek1, 2.00002);
//    ASSERT_EQ(kek1, 2.000009);
}

}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
