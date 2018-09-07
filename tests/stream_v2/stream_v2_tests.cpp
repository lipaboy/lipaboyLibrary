#include <iostream>
#include <vector>
#include <list>
#include <cstdint>

#include <iterator>
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>
#include <cstdio>

#include <fstream>

#include <functional>
#include <tuple>

#include <gtest/gtest.h>

#include "stream_v2/stream.h"

namespace stream_v2_tests {

	using std::cout;
	using std::endl;
	using std::vector;
	using std::list;
	using std::string;
	using std::unique_ptr;

	using namespace lipaboy_lib;
	using namespace lipaboy_lib::stream_v2_space;
	using namespace lipaboy_lib::stream_v2_space::operators_space;

	//---------------------------------Tests-------------------------------//


	//-------------------------------------//


	//----------Constructor-----------//

	TEST(Stream_v2, check) {
		vector<int> lol = { 1, 2, 3, 4 };
		auto kek = buildStream(lol.begin(), lol.end())
			| filter([](auto i) { return i % 2 == 0; })
			| nth(1);

		ASSERT_EQ(kek, 4);

		vector<int> lol2 = { 1, 2, 3, 4 };
		auto kek2 = buildStream(lol2.begin(), lol2.end())
			| group_by_vector(2)
			| nth(1);

		ASSERT_EQ(kek2, decltype(kek2)({ 3, 4 }));
	}

}