#ifndef HASH_MAP_TEST_H
#define HASH_MAP_TEST_H

#include <gtest/gtest.h>
#include "hash_map/hash_map.h"
#include <forward_list>

namespace hash_map_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unordered_map;
using hash_map_space::HashMap;
using hash_map_space::ForwardListStoragedSize;
using std::unique_ptr;

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

class HashMapTest : public ::testing::Test  {
public:
    typedef HashMap<int, string> HashMapType;
    typedef HashMapType::size_type size_type;
    typedef unique_ptr<HashMapType> HashMapTypePtr;
protected:
    void SetUp() {
        hashMap = std::make_unique<HashMapType>();
    }
    void TearDown() {}

    HashMapType::size_type& getRefSize() { return hashMap->size_; }
    // the same as bucket_count
    HashMapType::size_type containerSize() const { return hashMap->container.size(); }
    bool isBucketIncrease() const { return hashMap->isBucketIncrease(); }
    bool isBucketDecrease() const { return hashMap->isBucketDecrease(); }

    HashMapTypePtr hashMap;
};

class ForwardListStoragedSizeTest : public ::testing::Test  {
public:
    typedef ForwardListStoragedSize<string> ForwardListType;
    typedef unique_ptr<ForwardListType> ForwardListTypePtr;
protected:
    void SetUp() {
        list = std::make_unique<ForwardListType>();
    }
    void TearDown() {}

    ForwardListTypePtr list;
};

}

#endif // HASH_MAP_TEST_H
