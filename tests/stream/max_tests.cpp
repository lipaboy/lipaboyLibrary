#include <iostream>
#include <vector>
#include <string>

#include <functional>

#include <gtest/gtest.h>

#include "stream/stream.h"

namespace stream_tests {

	using std::cout;
	using std::endl;
	using std::vector;
	using std::string;

	using namespace lipaboy_lib;

	using namespace lipaboy_lib::stream;
	using namespace lipaboy_lib::stream::operators;

	//---------------------------------Tests-------------------------------//

	TEST(Stream_max, simple) {
		int a = 0;
		int max1 = buildStream([&a]() { return a++; }) | get(1000) | max_impl<int>(max<int>(-1));
		ASSERT_EQ(max1, 999);
	}

}

