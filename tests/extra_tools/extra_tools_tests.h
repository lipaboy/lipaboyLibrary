#ifndef EXTRA_TOOLS_TESTS_H
#define EXTRA_TOOLS_TESTS_H

#include "gtest/gtest.h"
#include <iostream>

namespace lipaboy_lib_tests {

using std::cout;
using std::endl;

class Noisy {
public:
    Noisy() { cout << "Constructed" << endl; }
    Noisy(Noisy const & ) { cout << "Copy-Constructed" << endl; }
    Noisy(Noisy && ) { cout << "Move-Constructed" << endl; }
    ~Noisy() { cout << "Destructed" << endl; }

    const Noisy& operator= (const Noisy&) {
        return *this;
    }
    const Noisy& operator= (const Noisy&&) {
        return *this;
    }
};

}

#endif // EXTRA_TOOLS_TESTS_H
