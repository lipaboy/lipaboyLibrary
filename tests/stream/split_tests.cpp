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

