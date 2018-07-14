#ifndef EXTRA_TOOLS_TESTS_H
#define EXTRA_TOOLS_TESTS_H

#include "gtest/gtest.h"
#include <iostream>

namespace lipaboy_lib_tests {

using std::cout;
using std::endl;

struct NoisyCon {
    NoisyCon() { cout << "Constructed" << endl; }
    NoisyCon(NoisyCon const &) {}
    NoisyCon(NoisyCon &&) {}
};

struct NoisyCopy {
    NoisyCopy() {}
    NoisyCopy(NoisyCopy const &) { cout << "Copy-Constructed" << endl; }
};

struct NoisyMove {
    NoisyMove() {}
    NoisyMove(NoisyMove &&) { cout << "Move-Constructed" << endl; }
};

// NoisyD - Noisy without notifying of destruction
struct NoisyD : NoisyCon, NoisyCopy, NoisyMove {
    NoisyD() {}
    NoisyD(NoisyD const & obj) : NoisyCopy(obj), NoisyCon(obj) {}
    NoisyD(NoisyD&& obj) : NoisyMove(std::move(obj)), NoisyCon(std::move(obj)) {}

    const NoisyD& operator= (const NoisyD&) {
        return *this;
    }
    const NoisyD& operator= (const NoisyD&&) {
        return *this;
    }
};

struct Noisy : NoisyD {
    virtual ~Noisy() { cout << "Destructed" << endl; }
};

}

#endif // EXTRA_TOOLS_TESTS_H
