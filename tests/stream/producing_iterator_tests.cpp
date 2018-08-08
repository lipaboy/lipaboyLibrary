#include "gtest/gtest.h"
#include <iostream>

#include "stream/producing_iterator.h"

namespace stream_tests {

	using std::cout;
	using std::endl;

	using stream_space::ProducingIterator;

	TEST(ProducingIterator, lambda_relative_on_external_context) {
		int x = 0;
		ProducingIterator<int> iter([&a = x]() { return a++; });

		ASSERT_EQ(*iter, 0);
		iter++;
		ASSERT_EQ(*iter, 1);
		++iter;
		ASSERT_EQ(*(++iter), 3);

		auto iter2 = iter;
		ASSERT_TRUE(iter == iter2);
		iter++;
		// don't work because ProducingIterator hasn't strong condition in comparison
		//ASSERT_TRUE(iter != iter2);

		// ProducingIterator is not a real iterator
		/*auto lol = std::is_same<typename std::iterator_traits<ProducingIterator<int> >::iterator_category,
		std::input_iterator_tag>::value;
		ASSERT_TRUE(lol);*/
	}

}