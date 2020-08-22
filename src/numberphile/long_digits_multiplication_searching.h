#ifndef LONG_DIGITS_MULTIPLICATION_SEARCHING_H
#define LONG_DIGITS_MULTIPLICATION_SEARCHING_H

#include "extra_tools/detect_time_duration.h"

#include <vector>
#include <iostream>

#include "stream/stream.h"

namespace lipaboy_lib::numberphile {

    using std::vector;
    using std::cout;
    using std::endl;

    using namespace lipaboy_lib::stream_space;
    using namespace lipaboy_lib::stream_space::operators;

    template <typename T>
    inline T pow(T number, T degree) {
        T result = static_cast<T>(1);
        for (T i = static_cast<T>(0); i < degree; i++) {
            result *= number;
        }
        return result;
    }


    void long_digits_multiplication_searching_vectors();

    void long_digits_multiplication_searching_uint64_t();

    void long_digits_multiplication_searching_optimized();

}


#endif // LONG_DIGITS_MULTIPLICATION_SEARCHING_H
