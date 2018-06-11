#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <string>

#include "common_interfaces/algebra.h"

#include "stream_test.h"

namespace check_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;

template <class Derived>
struct Base {
    void callSmth() {
        static_cast<Derived*>(this)->lol();
    }
};

struct Derived : Base<Derived> {
    void lol() {
        cout << "lol" << endl;
    }
};

struct Derived2 : Base<Derived2> {
    void lol() {
        cout << "lol 2" << endl;
    }
};


TEST(Check, check) {
    Derived d;
    d.callSmth();
    Derived2 d2;
    d2.callSmth();
}

}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
