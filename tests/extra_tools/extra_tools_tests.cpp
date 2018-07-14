#include "extra_tools_tests.h"

#include "extra_tools/extra_tools.h"

namespace lipaboy_lib_tests {

using lipaboy_lib::relativeForward;

class A {
public:
    A() {}
    template <class A_>
    A(int e, A_&& a)
        : n(relativeForward<A_, Noisy>(a, a.n))
    {
//        cout << std::is_lvalue_reference<A_&&>::value << std::is_rvalue_reference<A_&&>::value << endl;
//        cout << std::is_lvalue_reference<A_>::value << std::is_rvalue_reference<A_>::value << endl;
//        cout << std::is_lvalue_reference<decltype(a)>::value
//             << std::is_rvalue_reference<decltype(a)>::value << endl;
//        cout << std::is_lvalue_reference<decltype(a.n)>::value
//             << std::is_rvalue_reference<decltype(a.n)>::value << endl;
    }

    using NoisyType = Noisy;
    Noisy n;
};

TEST(Check, check3) {
    A a;
    A a2 = A(5, a);
    A a3 = A(3, std::move(a));
}

}
