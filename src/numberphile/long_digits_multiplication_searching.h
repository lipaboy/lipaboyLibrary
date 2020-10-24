#ifndef LONG_DIGITS_MULTIPLICATION_SEARCHING_H
#define LONG_DIGITS_MULTIPLICATION_SEARCHING_H

#include "extra_tools/detect_time_duration.h"

#include <vector>
#include <iostream>

#include "stream/stream.h"
#include "long_numbers/long_number.h"
#include "extra_tools/maths_tools.h"

namespace lipaboy_lib::numberphile {

    using std::vector;
    using std::cout;
    using std::endl;

    using namespace lipaboy_lib::stream_space;
    using namespace lipaboy_lib::stream_space::operators;
    using lipaboy_lib::long_numbers_space::LongIntegerDecimal;

    void long_digits_multiplication_searching_vectors();

    void long_digits_multiplication_searching_uint64_t();

    void long_digits_multiplication_searching_optimized();

    void long_digits_multiplication_searching_long_numbers();

}


#endif // LONG_DIGITS_MULTIPLICATION_SEARCHING_H
