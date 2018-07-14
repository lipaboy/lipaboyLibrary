#include "extra_tools_tests.h"

#include "extra_tools/extra_tools.h"

namespace lipaboy_lib_tests {

using lipaboy_lib::relativeForward;
using lipaboy_lib::RelativeForward;

namespace {

class A {
public:
    A() {}

    template <class A_>
    A(int e, A_&& a)
        : //n(relativeForward<A_, Noisy>(std::forward<A_>(a), a.n))
          n(RelativeForward<A_&&, NoisyD>::forward(a.n))
    {
//        cout << std::is_lvalue_reference<A_&&>::value << std::is_rvalue_reference<A_&&>::value << endl;
//        cout << std::is_lvalue_reference<A_>::value << std::is_rvalue_reference<A_>::value << endl;
//        cout << std::is_lvalue_reference<decltype(a)>::value
//             << std::is_rvalue_reference<decltype(a)>::value
//             << std::is_object<decltype(a)>::value
//             << endl;
//        cout << std::is_lvalue_reference<decltype(a.n)>::value
//             << std::is_rvalue_reference<decltype(a.n)>::value
//             << std::is_object<decltype(a.n)>::value
//             << endl;
    }

    NoisyD n;
};

//template <class A_>
//bool isLValue(A_&& a) {
//    std::is_lvalue_reference<decltype(relativeForward<A_, Noisy>)>
//}

}

TEST(RelativeForward, check) {
    A a;
    A a2 = A(5, a);
    A a3 = A(3, std::move(a));
    A a4 = A(1, A());
}

}

