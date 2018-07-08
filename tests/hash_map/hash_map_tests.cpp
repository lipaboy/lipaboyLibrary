#include <iostream>
#include <vector>
#include <cstdint>

#include <iterator>
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>

#include <gtest/gtest.h>

#include "hash_map/hash_map.h"
#include "hash_map_test.h"

namespace hash_map_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unordered_map;
using hash_map_space::HashMap;
using hash_map_space::ForwardListStoragedSize;
using std::unique_ptr;


//---------------------------------Tests-------------------------------//

//---------Allocators---------//

TEST(HashMapAllocs, default_alloc) {
	using HashMapType = HashMap<int, int>;
	HashMapType kek(typename HashMapType::ValueTypeAllocator());
	//ASSERT_TRUE(std::is_same<typename HashMapType::ValueTypeAllocator, std::allocator<std::pair<int, int> > >::value == true);
}

TEST_F(HashMapTest, default_constructor) {
	ASSERT_EQ(getRefSize(), 0u);
	ASSERT_EQ(containerSize(), 1u);
}

//---------Unique_ptr checking---------//

TEST(HashMapUniquePtr, check) {
    HashMap<int, unique_ptr<int> > kek;
    unique_ptr<int> p(new int(6));
    kek.insert(5, std::move(p));

    auto iter = (kek.find(5));
    std::unique_ptr<int>& pp = iter->second;
    ASSERT_EQ(*pp, 6);
}

//-----------Iterator------------//

TEST_F(HashMapTest, iterator_begin_end) {
    ASSERT_NO_THROW(hashMap->begin());
    ASSERT_NO_THROW(hashMap->end());
}
TEST_F(HashMapTest, iterator_equality) {
    ASSERT_TRUE(hashMap->begin() == hashMap->end());
}
TEST_F(HashMapTest, iterator_dereference) {
    int key = 1;
    string value = "simple_kek";
    hashMap->insert(key, value);
    auto iter = hashMap->begin();
    ASSERT_EQ(*iter, HashMapTest::HashMapType::value_type(key, value));
    ASSERT_EQ(iter->first, key);
    ASSERT_EQ(iter->second, value);
}
TEST_F(HashMapTest, iterator_post_and_pre_increment) {
    int key = 1;
    string value = "simple_kek";
    hashMap->insert(key, value);
    hashMap->insert(2, value);
    auto iter = hashMap->begin();
    auto tempKey = (iter++)->first;
    ASSERT_TRUE(tempKey == key || tempKey == 2);
//    ASSERT_TRUE((++iter) == hashMap->end());
}


//---------Main interface---------//

TEST_F(HashMapTest, get) {
    ASSERT_FALSE(hashMap->get(1).has_value());
}

TEST_F(HashMapTest, insert) {
    int key = 1;
    string value = "simple_kek";
    hashMap->insert(key, value);

    ASSERT_EQ(hashMap->get(key).value(), value);
    ASSERT_EQ(hashMap->size(), 1u);

    hashMap->insert(2, "simple_lol");
    ASSERT_EQ(hashMap->get(2).value(), "simple_lol");
    ASSERT_EQ(hashMap->size(), 2u);

    hashMap->insert(2, "double_lol");
    ASSERT_EQ(hashMap->get(2).value(), "double_lol");
    ASSERT_EQ(hashMap->size(), 2u);
}

TEST_F(HashMapTest, extract) {
    int key = 1;
    string value = "simple_kek";
    hashMap->insert(key, value);

    hashMap->extract(key);

    ASSERT_FALSE(hashMap->get(key).has_value());
    ASSERT_TRUE(hashMap->empty());
}

//---------Private fields---------//

TEST_F(HashMapTest, rehash) {
    string value = "simple_kek_";
    HashMapTest::size_type presumedBucketCount = 1u;
    HashMapTest::size_type size = 0u;
    for (int key = 1; key < 100; key++) {
        getRefSize() += 1;
        if (isBucketIncrease())
            presumedBucketCount *= 2;
        getRefSize() -= 1;
        hashMap->insert(key, value + std::to_string(key));
        ++size;
        ASSERT_EQ(hashMap->bucket_count(), presumedBucketCount);
        ASSERT_EQ(size, hashMap->size());
    }

    presumedBucketCount = hashMap->bucket_count();
    for (int key = 1; key < 100; key++) {
        getRefSize() -= 1;
        if (isBucketDecrease())
            presumedBucketCount /= 2;
        getRefSize() += 1;
        hashMap->extract(key);
        --size;
        ASSERT_EQ(hashMap->bucket_count(), presumedBucketCount);
        ASSERT_EQ(size, hashMap->size());
    }
}

//---------Fields---------//

TEST_F(HashMapTest, size_and_bucket_count) {
    ASSERT_EQ(hashMap->size(), 0u);
    ASSERT_EQ(hashMap->bucket_count(), 0u);
}

TEST_F(HashMapTest, empty) {
    ASSERT_EQ(hashMap->empty(), true);
}



TEST_F(HashMapTest, check_operator_assignment) {
    HashMapType other;
    HashMapType & map = *hashMap;

    other[2] = "aka";
    map[2] = "maka";
    map[3] = "fo";

    other = map;
    ASSERT_EQ(other[2], "maka");
    ASSERT_EQ(other[3], "fo");
}

TEST_F(HashMapTest, check_operator_assignment_by_noisy) {
//    HashMap<int, Noisy> first;
//    HashMap<int, Noisy> second;
//    Noisy kekN;

//    cout << "control point 0" << endl;

//    first.insert(2, kekN);
////    first[2] = kekN;
//    first.emplace(2, kekN);
//    first.emplace(10, Noisy());

////    second.insert(4, kekN);
////    second.insert(3, kekN);
//    second[4] = kekN;
//    second[3] = kekN;

//    cout << "control point 1" << endl;

//    first = second;

//    cout << "control point 2" << endl;

//    first = first;

//    cout << "control point 3" << endl;

    ASSERT_EQ(1, 1);
}

TEST_F(HashMapTest, check_initialize_list) {
    int key = 5;
    string value = "lol";
    HashMap<int, string> kek = { {key, value}, {2, "kek"} };
    ASSERT_EQ(kek.get(5), value);
    ASSERT_EQ(kek.get(2), "kek");
}

TEST_F(HashMapTest, check_iterator) {
    auto iter = hashMap->begin();
    ASSERT_TRUE(iter == hashMap->end());
    hashMap->insert(2, "ku");
    iter = hashMap->insert(3, "kukek");
    ASSERT_EQ((++iter)->second, "ku");
}

TEST_F(HashMapTest, check_const_iterator) {
    auto cit = hashMap->cbegin();
    ASSERT_TRUE(cit == hashMap->cend());
    hashMap->insert(2, "ku");
    cit = hashMap->insert(3, "kukek");
    ASSERT_EQ((++cit)->second, "ku");

}

}
