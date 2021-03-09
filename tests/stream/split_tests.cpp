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

	using namespace lipaboy_lib::stream_space;
	using namespace lipaboy_lib::stream_space::op;

	//---------------------------------Tests-------------------------------//

	TEST(Stream_Split, strings) {
		string str = "hello, world!!";
		auto outStr = Stream(str.begin(), str.end()) 
			| split<string>(
				[](char ch) -> bool 
				{ 
					return ch == ' ' || ch == ',' || ch == '!'; 
				}) 
			| to_vector();
		ASSERT_EQ(outStr, vector<string>({ "hello", "world", "" }));
	}

	TEST(Stream_Split, vectors) {
		string str = "hello, world!";
		auto outStr = Stream(str.begin(), str.end())
			| split< vector<char> >(
				[](char ch) -> bool 
				{ 
					return ch == ' '; 
				})
			| map(
				[](vector<char> const & vec) 
				{ 
					return std::string(vec.begin(), vec.end()); 
				})
            | sum();

        ASSERT_EQ(outStr, "hello,world!");
	}

	TEST(Stream_Split, group_by_numbers) {
		string str = "my pocket money in GTAV: 5 234 321$ but my brother has 5 324$";
		auto val = Stream(str)
			| filter(
				[](char ch) -> bool
				{
					return ch != ' ';
				})
			| split< std::string >(
				[](char ch) 
				{
					return !std::isdigit(ch);
				})
			| map(
				[](std::string const & numStr) -> int
				{
					return std::stoi(numStr);
				})
			| sum();
		ASSERT_EQ(val, 5239645);
	}

}

