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

	TEST(Stream_nop, simple) {
        auto stream = Stream(1, 2, 3, 4, 5)
                    | nop();
        stream | return_zero();
        int zero = Stream(1, 2, 3, 4, 5)
            | nop()
            | return_zero();
        ASSERT_EQ(zero, 0);

        zero = Stream(vector<int>{1, 2, 3, 4, 5})
            | nop()
            | return_zero();
        ASSERT_EQ(zero, 0);
	}

}
