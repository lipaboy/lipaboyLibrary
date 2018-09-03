#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <any>
#include <list>
#include <tuple>

#include "extra_tools/extra_tools.h"

#include "stream/stream_test.h"
#include "extra_tools/extra_tools_tests.h"

namespace lipaboy_lib_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unique_ptr;
using std::any;




// -------------- Doesn't work------------//
//struct A {
//	template <class A_>
//	A(A_&& obj, void* p) 
//		: a(lipaboy_lib::RelativeForward<A_, string>::forward(obj.a))
//	{}
//
//	A(const A& obj) :  A<const A&>(obj, nullptr)
//	{}
//	A(A&& obj) :  A<A&&>(std::move(obj), nullptr)
//	{}
//
//	string a;
//};

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

TEST(Check, check) {
	using std::get;

	auto res = combine(1, "lol");

	ASSERT_EQ(get<0>(res), 1);
	ASSERT_EQ(get<1>(res), std::string("lol"));

	auto res2 = combine2(std::list<int>({ 1, 2, 3 }), std::list<string>({ "lol", "kek" }));

	ASSERT_TRUE([]() -> bool { return true; }());

	ASSERT_EQ(get<0>(res2.front()), 2);
	ASSERT_EQ(get<1>(res2.front()), "kek");
}

}



int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
	auto res = RUN_ALL_TESTS();
#ifdef WIN32
	system("pause");
#endif
    return res;
}
