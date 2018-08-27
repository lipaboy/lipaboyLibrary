#pragma once

#include <array>
#include <cstdint>

#include "extra_tools/extra_tools.h"

namespace lipaboy_lib {

namespace long_numbers_space {

// PLAN
// ----
// TODO: think about making IntegralType and TResult the template parameters.
// TODO: write constructor which will be parse the std::string of number (Deserialization).
// TODO: write the method to serialization the number into std::string.

using std::array;
using std::int32_t;
using std::int64_t;

template <size_t lengthOfIntegrals,     // count of integral type variables
          class TIntegral = int32_t,
          class TResult = int64_t>
class LongInteger
{
public:
    using IntegralType =
        std::remove_reference_t<
            lipaboy_lib::enable_if_else_t<2 * sizeof(TIntegral) == sizeof(TResult), TIntegral, void> >;
    using ResultType = std::remove_reference_t<TResult>;

public:
    explicit
    LongInteger(int32_t small) {
        number_[0] = small;
        for (size_t i = 1; i < number_.size(); i++)
            number_[i] = TIntegral(0);
    }


private:
    array<IntegralType, lengthOfIntegrals> number_;
};

}

}
