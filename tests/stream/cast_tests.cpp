#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cmath>

#include <gtest/gtest.h>

#include "stream_test.h"

namespace stream_tests {

	using std::cout;
	using std::endl;
	using std::vector;
	using std::string;
	using std::shared_ptr;

	using namespace lipaboy_lib::stream_space;
	using namespace lipaboy_lib::stream_space::op;


	TEST(Stream_Cast, simple) {
		char a = 0;
		auto res = Stream([&a]() { return a++; })
			| cast_to<int>()
			| get(5)
			| sum();

		ASSERT_EQ(res, 10);

		a = 0;
		res = Stream([&a]() { return a++; })
			| cast_static<int>()
			| get(5)
			| sum();

		ASSERT_EQ(res, 10);
	}

	TEST(Stream_Cast, cast_dynamic) {
		struct A {
			virtual int get() {
				return 5;
			}
			virtual ~A() {}
		};
		struct B : A {
			int get() {
				return -5;
			}
		};

		auto res = Stream([]() { return new B(); })
			| cast_dynamic<A*>()
			| get(3)
			| reduce(
				[](int res, A* a) {
					res += a->get();
					delete a;
					return res;
				},
				[](A* a) -> int {
					auto temp = a->get();
					delete a;
					return temp;
				}
			);

		ASSERT_EQ(res.value(), -15);
	}

	TEST(Stream_Cast, smart_ptr) {
		struct A {
			virtual int get() {
				return 5;
			}
			virtual ~A() {}
		};
		struct B : A {
			int get() {
				return -5;
			}
		};

		auto res = Stream([]() { return std::make_shared<B>(); })
			| cast_to<shared_ptr<A> >()
			| get(3)
			| reduce(
				[](int res, shared_ptr<A> a) {
					return res + a->get();
				},
				[](shared_ptr<A> a) -> int {
					return a->get();
				}
				);

		ASSERT_TRUE(res < 0);

	}

	bool isKek(int) { return true; }
	bool isKek(double) { return false; }
	int isLol(double) { return 2; }
	bool isLol2(double) { return bool(2); }
	template <class Predicate>
	void foo(Predicate p) {
		static_cast<bool (*)(double)>(&p);
	}

	TEST(Stream_Cast, troubles) {

		double a = 0.;
		Stream([&a]() { return a++; })
            | map(static_cast<double (*)(double)>(&std::floor))
			| get(3)
			| sum();

		a = 0.;
		Stream([&a]() { return a++; })
			| get(2)
			| filter<bool(double)>(isKek)
			| sum();

		a = 0.;
		Stream([&a]() { return a++; })
			| get(2)
			| filter(isLol)
			| sum();

		// Compile-error
		/*a = 0.;
		Stream([&a]() { return a++; })
			| filter(isKek)
			| get(2)
			| sum();*/

		
	}

}

