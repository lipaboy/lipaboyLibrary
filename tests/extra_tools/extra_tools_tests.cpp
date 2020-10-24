#include "extra_tools_tests.h"

#include "extra_tools/extra_tools.h"

#include <iterator>
#include <string>
#include <iostream>

namespace lipaboy_lib_tests {

using lipaboy_lib::RelativeForward;
using lipaboy_lib::WrapBySTDFunctionExcludeLambdaType;
using lipaboy_lib::WrapBySTDFunctionType;

using std::string;
using std::cout;
using std::endl;

namespace {

template <class T>
class A {
public:
    using value_type = T;
public:
    A() : n(T()) {}

    template <class A_>
    A(int e, A_&& a)
          : n(RelativeForward<A_&&, T>::forward(a.n))
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

    T n;
};

template <class A_>
using Briefly = RelativeForward<A_&&, typename std::remove_reference_t<A_>::value_type>;

template <class A_>
bool isLValueRef(A_&& a) {
    return std::is_lvalue_reference<
            decltype(Briefly<A_&&>::forward(a.n))>::value;
}

template <class A_>
bool isRValueRef(A_&& a) {
    return std::is_rvalue_reference<
            decltype(Briefly<A_&&>::forward(a.n))>::value;
}

}

TEST(RelativeForward, noisy_test) {
//    using type = A<NoisyD>;
//    type a;
//    type a2 = type(5, a);
//    type a3 = type(3, std::move(a));
//    type a4 = type(1, type());
}

TEST(RelativeForward, simple_test) {
    A<int> temp;
    ASSERT_TRUE(isLValueRef(temp));
    ASSERT_FALSE(isLValueRef(std::move(temp)));
    ASSERT_TRUE(isRValueRef(std::move(temp)));
    ASSERT_FALSE(isRValueRef(temp));
}


namespace {
	int sumKek(int) {
		return 1;
	}

	template <class F>
	class S {
	public:
		S(F f) : f_(f) {}
		void justDo() { std::cout << ""; }

		WrapBySTDFunctionExcludeLambdaType<F> f_;
	};
}

TEST(Wrap_By_StdFunction, excluding_lambdas) {
	auto func
		= [](auto) { return false; };

	// logical error
	//WrapBySTDFunctionExcludeLambdaType<decltype(func)> func2 = [](auto) { return false; };

	// logical error (type of lambda has an unique name)
	//decltype([](auto) { return false; }) func2 = [](auto) { return false; };

	func(1);

	auto func4 = [](int) { return 4; };
	WrapBySTDFunctionType<decltype(func4)> func5 = func4;

	S s1([](auto) { return false; });
	s1.justDo();
	S s2(func);
	s2.justDo();

	std::function<int(int)> func3 = sumKek;
}

}

