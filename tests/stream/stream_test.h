#ifndef HASH_MAP_TEST_H
#define HASH_MAP_TEST_H

#include <gtest/gtest.h>
#include "stream/stream.h"
#include <forward_list>

namespace stream_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unique_ptr;
using stream_space::Stream;

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

//class StreamTest : public ::testing::Test  {
//public:
//   // typedef Stream<int, boost::random_access_traversal_tag> StreamInt;
//    //typedef unique_ptr<StreamInt> StreamIntPtr;

//protected:
//    void SetUp() {
//       // pStream = std::unique_ptr<StreamInt>(new StreamInt({}));
//    }
//    void TearDown() {}

//protected:
//   // StreamIntPtr pStream;
//};

}

#endif // HASH_MAP_TEST_H
