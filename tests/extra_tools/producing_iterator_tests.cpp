#include "gtest/gtest.h"
#include <iostream>

#include "extra_tools/producing_iterator.h"
#include "extra_tools/sequence_producing_iterator.h"

namespace stream_tests {

	using namespace lipaboy_lib;

	using std::cout;
	using std::endl;


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

	TEST(SequenceProducingIterator, simple) {
		SequenceProducingIterator<int> it(0, [](int prev) { return prev + 1; });
		for (int i = 0; i < 3; i++)
			it++;
		ASSERT_EQ(*it, 3);
		for (int i = 0; i < 3; i++)
			EXPECT_EQ(*(++it), i + 4);
	}

}