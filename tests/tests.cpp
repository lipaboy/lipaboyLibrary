#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <list>
#include <tuple>
#include <functional>
#include <algorithm>

//#include "extra_tools/extra_tools.h"

#include "stream/stream.h"
//#include "extra_tools/extra_tools_tests.h"
//#include "extra_tools/maths_tools.h"
//#include "maths/fixed_precision_number.h"

#include "extra_tools/detect_time_duration.h"

#include "numberphile/long_digits_multiplication_searching.h"

namespace lipaboy_lib_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unique_ptr;
using std::any;

using namespace lipaboy_lib;
using namespace lipaboy_lib::stream_space;
using namespace lipaboy_lib::stream_space::operators;


namespace {

	template <class... Args>
	decltype(auto) combine(Args... arg) {
		return std::make_tuple(arg...);
	}

	template <class... Args>
	decltype(auto) combine2(std::list<Args>... list) {
		std::list<std::tuple<Args...> > res;
		size_t const size = std::min({ list.size()... });

		//for (auto & elem : list) 
		//cout << size << endl;

		for (size_t i = 0; i < size; i++) {
			res.push_front(std::tuple<Args...>(list.front()...));
			(list.pop_front(), ...);
		}

		return res;
	}

}


TEST(Check, check) {

}

}


//#define LIPABOY_LIB_TESTING

int main(int argc, char *argv[])
{
#ifdef LIPABOY_LIB_TESTING
    ::testing::InitGoogleTest(&argc, argv);
	auto res = RUN_ALL_TESTS();

    return res;
#else
    using namespace lipaboy_lib::numberphile;

    long_digits_multiplication_searching_vectors();
    long_digits_multiplication_searching_uint64_t();
    long_digits_multiplication_searching_optimized();
    long_digits_multiplication_searching_long_numbers();

    return 0;
#endif
}
