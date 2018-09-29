#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <any>
#include <list>
#include <tuple>
#include <functional>
#include <algorithm>

#include <time.h>

#include "extra_tools/extra_tools.h"

#include "stream/stream_test.h"
#include "extra_tools/extra_tools_tests.h"
#include "extra_tools/maths_tools.h"

#include "maths/vector4d.h"

namespace lipaboy_lib_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unique_ptr;
using std::any;

using namespace lipaboy_lib;


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


TEST(LuckyTicket, DISABLED_two_lucky_ones_in_sequence)
{
	using namespace fast_stream;
	using namespace fast_stream::operators;

	vector<int> ticket2(6, 0);
	bool isLucky2 = false;

	auto start2 = clock();
	for (int i = 0; i < 1000000; i++) {
		for (int j = 0; j < 6; j++) {
			ticket2[j] = (i % powDozen<int>(j + 1)) / powDozen<int>(j);
		}
		int sum1 = ticket2[0] + ticket2[1] + ticket2[2];
		int sum2 = ticket2[3] + ticket2[4] + ticket2[5];
		if (sum1 == sum2) {
			/*if (isLucky2)
				(buildStream(ticket2) | print_to(cout, "")) << endl;
			else*/
				isLucky2 = true;
		}
		else
			isLucky2 = false;
	}
	cout << clock() - start2 << endl;

	vector<int> ticket(6, 0);
	bool isLucky = false;

	auto start1 = clock();
	for (int i = 0; i < 1000000; i++) {
		for (int j = 0; j < 6; j++) {
			ticket[j] = (i % powDozen<int>(j + 1)) / powDozen<int>(j);
		}
		int sum1 = ticket[0] + ticket[1] + ticket[2];
		//int sum1 = buildStream(ticket) | get(3) | sum();
		int sum2 = buildStream(ticket) | skip(3) | sum();
		//int sum2 = ticket[3] + ticket[4] + ticket[5];
		if (sum1 == sum2) {
			/*if (isLucky)
				(buildStream(ticket) | print_to(cout, "")) << endl;
			else*/
				isLucky = true;
		}
		else
			isLucky = false;
	}
	cout << clock() - start1 << endl;

	

	//ASSERT_FALSE(true);
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
