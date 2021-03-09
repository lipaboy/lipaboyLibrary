#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <functional>
#include <tuple>

#include <gtest/gtest.h>

#include "stream/stream.h"

namespace stream_tests {

    using std::cout;
    using std::endl;
    using std::vector;
    using std::string;
    using std::tuple;
    using std::get;

    using namespace lipaboy_lib;

    using namespace lipaboy_lib::stream_space;
    using namespace lipaboy_lib::stream_space::op;

    // TODO: wrap the tuple by the concrete struct (like {name:, age:})

    //------------------------------Tests-------------------------------//

    TEST(StreamTest, tupled_stream_double) {
        auto first = Stream(1, 2, 3);
        auto second = Stream(-1, -2, -3);
        auto res = to_tuple(first, second)
            | map([](auto t) { return get<0>(t) + get<1>(t); })
            | sum();

        ASSERT_EQ(0, res);
    }

    TEST(StreamTest, tupled_stream_triple) {
        auto first = Stream(2, 2, 8);
        auto second = Stream(1, 2, 3);
        auto third = Stream(-1, -2, -3);
        auto res = to_tuple(first, second, third)
            | map([](auto t) { return get<0>(t) + get<1>(t) + get<2>(t); })
            | sum();

        ASSERT_EQ(12, res);
    }

    TEST(StreamTest, tupled_increment) {
        auto first = Stream(2, 2, 8);
        auto second = Stream(1, 2, 3);
        auto res = to_tuple(first, second)
            | skip(2)
            | map([](auto t) { return get<0>(t) + get<1>(t); })
            | sum();

        ASSERT_EQ(res, 11);
    }

    //--------------------Context of tupled realization---------------------//

    namespace {

        template <class T1, class... Rest>
        struct A : A<Rest...> {
            A() = default;

            // Variadic template constructor
            template <class _T1, class... _Rest>
            A(_T1 t1, _Rest... rest)
                : value(t1), A<_Rest...>(rest...)
            {}

            int count() {
                return A<Rest...>::count() + 1;
            }

            T1 value;
        };

        template <class T>
        struct A<T> {
            A(T kek) : value(kek) 
            {}

            int count() {
                return 1;
            }

            T value;
        };

    }

    TEST(StreamTest, tupled_stream_context) {
        A<bool, int, double, string> a(true, 5, 2.5, string("kek"));
        ASSERT_EQ(a.count(), 4);
    }

}

