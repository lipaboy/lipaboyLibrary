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

#ifdef STREAM_V1_TESTS_RUN
	using namespace lipaboy_lib::stream_v1_space;
	using namespace lipaboy_lib::stream_v1_space::operators_space;
#else
	using namespace lipaboy_lib::stream;
	using namespace lipaboy_lib::stream::operators;
#endif

	//---------------------------------Tests-------------------------------//


	//-------------------------------------//


	//----------Group-----------//

	TEST(Split, strings) {
		string str = "hello, world!";
		string outStr = buildStream(str.begin(), str.end()) | split([](char ch) -> bool { return ch == ' '; }) | sum();
		ASSERT_EQ(outStr, "hello,world!");
	}

	TEST(Split, vectors) {
		string str = "hello, world!";
		string outStr = buildStream(str.begin(), str.end()) 
			| split<std::function<bool(char)>, vector<char> >([](char ch) -> bool { return ch == ' '; }) 
			| map([](vector<char> const & vec) { return std::string(vec.begin(), vec.end()); })
			| sum();
		ASSERT_EQ(outStr, "hello,world!");
	}

}

