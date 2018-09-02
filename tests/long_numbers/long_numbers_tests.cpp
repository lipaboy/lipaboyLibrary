#include <gtest/gtest.h>

#include "long_numbers/long_number.h"

namespace long_numbers_tests {

using namespace lipaboy_lib;

using long_numbers_space::LongIntegerDecimal;

TEST(LongInteger, check) {
	LongIntegerDecimal<4> num = "123456789012345690";
}

}
