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
#include "maths/fixed_precision_number.h"

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
