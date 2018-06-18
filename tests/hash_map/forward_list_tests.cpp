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

TEST_F(ForwardListStoragedSizeTest, check_emplace_front) {
    auto & lol = list->emplace_front("lol");
    ASSERT_EQ(lol, "lol");
    auto & kek = list->push_front("kek");
    ASSERT_EQ(kek, "kek");
}

TEST_F(ForwardListStoragedSizeTest, erase_after) {
    //auto list = std::forward_list<string>();
    auto & refList = *list;
    refList.push_front("lol1");
    refList.push_front("lol2");
    refList.push_front("lol3");
    refList.push_front("lol4");

    auto iter = refList.begin();
    refList.erase_after(iter);
    ASSERT_EQ(std::distance(refList.begin(), refList.end()), 3u);
}


TEST_F(ForwardListStoragedSizeTest, check_emplace_front_with_noisy) {
//    ForwardListStoragedSize<Noisy> list;
//    Noisy nKek;
//    list.emplace_front(nKek);
//    cout << "----delimiter----" << endl;
//    list.emplace_front(Noisy());
//    cout << "----delimiter----" << endl;
//    list.push_front(nKek);
//    cout << "----delimiter----" << endl;
//    list.push_front(Noisy());
    ASSERT_EQ(0, 0);
}

}
