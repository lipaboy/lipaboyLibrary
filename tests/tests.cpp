#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <any>
#include <list>
#include <tuple>
#include <functional>

#include "extra_tools/extra_tools.h"

#include "stream/stream_test.h"
#include "extra_tools/extra_tools_tests.h"

#include "maths/vector4d.h"

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

	//std::function<bool(string)> f = [](string & r) { return false; };

	string kek1 = "kek1";
	string kek2 = "kek2";
	string kek3 = "kek3";
	std::unordered_set<std::reference_wrapper<const string>, 
		std::hash<string>, std::equal_to<const string> > olo;
	olo.insert(std::cref(kek1));
	olo.insert(std::cref(kek2));

	ASSERT_NE(olo.find(kek1), olo.end());
	ASSERT_EQ(olo.find(kek3), olo.end());

	const int & kek = 5;
	auto lol = std::move(kek);

	vector<int> kra{ 1, 1, 2, 3, 1, 1, 2, 4 };
	std::unordered_set<std::reference_wrapper<const int>,
		std::hash<int>, std::equal_to<const int> > set;
	set.insert(kra.begin(), kra.end());
	vector<int> res3;
	for (int i = 1; i <= 4; i++)
		if (set.find(std::cref(i)) != set.end())
			res3.push_back(i);
	ASSERT_EQ(res3, decltype(res3)({ 1, 2, 3, 4 }));

	double kle[4] = { 1., 2., 3., 4. };
	lipaboy_lib::Vector4D vect(kle);
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
