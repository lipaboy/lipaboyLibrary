#include "gtest/gtest.h"
#include <iostream>
#include <vector>
#include <string>

#include "stream/initializer_list_iterator.h"

namespace stream_tests {

	using namespace lipaboy_lib;

	using std::cout;
	using std::endl;
	using std::vector;
	using std::string;

	using stream_space::InitializerListIterator;

	TEST(InitializingListIterator, simple) {
		InitializerListIterator<string> iter({ "a", "b", "c", "d", "e" });

		ASSERT_EQ(*(iter++), "a");
		ASSERT_EQ(*iter, "b");
		++iter;
		ASSERT_EQ(*(++iter), "d");

		auto iter2 = iter;

		ASSERT_EQ(*(iter2++), "d");
		ASSERT_NE(iter, iter2);
		iter++;
		ASSERT_EQ(iter, iter2);

		// no difference_type
		auto lol = std::is_same<typename std::iterator_traits<InitializerListIterator<string> >::iterator_category,
			std::input_iterator_tag>::value;
		ASSERT_TRUE(lol);
	}

}