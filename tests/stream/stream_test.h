#ifndef HASH_MAP_TEST_H
#define HASH_MAP_TEST_H

#include "gtest/gtest.h"
#include "stream/stream.h"
#include <forward_list>

namespace stream_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unique_ptr;

using stream_space::Stream;
using stream_space::IsOutsideIteratorsRefer;

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

class OutsideItersStreamTest : public ::testing::Test  {
public:
    using Container = vector<int>;
    using StreamInt = Stream<IsOutsideIteratorsRefer, typename Container::iterator>;
    using StreamIntPtr = unique_ptr<StreamInt>;

protected:
    void SetUp() {
        pOutsideContainer = std::unique_ptr<Container>(new Container({ 1, 2, 3, 4, 5 }));
        pStream = std::unique_ptr<StreamInt>(stream_space::makeStream(pOutsideContainer->begin(),
                                                                      pOutsideContainer->end()));
    }
    void TearDown() {}

protected:
    StreamIntPtr pStream;
    unique_ptr<Container> pOutsideContainer;
};

}

#endif // HASH_MAP_TEST_H
