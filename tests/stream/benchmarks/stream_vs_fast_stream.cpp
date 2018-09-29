#include <gtest/gtest.h>

#include <string>
#include <chrono>

#include "stream/stream.h"

namespace stream_benchmarks {

	using namespace lipaboy_lib;

	using std::string;
	using std::chrono::steady_clock;

	decltype(auto) getCurrentTime() { return steady_clock::now(); }
	template <class ratio = std::chrono::milliseconds>
	decltype(auto) diffFromNow(steady_clock::time_point from) { 
		return std::chrono::duration_cast<ratio>(steady_clock::now() - from).count(); 
	}

	// Results: (Windows)
	// 7% boost and that's all
	TEST(Benchmark_stream_vs_fast_stream, DISABLED_skip) {
		const size_t SIZE = static_cast<size_t>(1e7);
		{
			auto start = getCurrentTime();
			int sum = 0;
			for (size_t i = 0; i < SIZE; i++)
				sum += i;
			cout << "Time: " << diffFromNow(start) << " Simple" << endl;
		}

		{
			using namespace stream;
			using namespace stream::operators;

			auto start = getCurrentTime();
			int a = 0;
			buildStream([&a]() { return a++; }) | skip(100) | get(SIZE) | sum();
			cout << "Time: " << diffFromNow(start) << " Stream" << endl;
		}

		{
			using namespace fast_stream;
			using namespace fast_stream::operators;

			auto start = getCurrentTime();
			int a = 0;
			buildStream([&a]() { return a++; }) | skip(100)
				| get(SIZE) | sum();
			cout << "Time: " << diffFromNow(start) << " Fast Stream" << endl;
		}

		ASSERT_FALSE(true);
	}

	TEST(Benchmark_stream_vs_fast_stream, skip_string) {
		const size_t SIZE = static_cast<size_t>(8e5);
		string sum1 = "";
		string sum2 = "";
		string sum3 = "";
		{
			int a = 0;
			auto gen = [&a]() { return std::to_string(a++); };
			//auto start = clock();
			auto start = getCurrentTime();
			auto lol = getCurrentTime();
			for (size_t i = 0; i < SIZE; i++) {
				if (i >= 100)
					sum1 += gen();
				else
					gen();
				lol = getCurrentTime();
			}
			cout << "Time: " << diffFromNow(start) << " Simple" << diffFromNow(lol) << endl;
		}

		{
			using namespace stream;
			using namespace stream::operators;

			auto start = getCurrentTime();
			int a = 0;
			sum2 = std::move(buildStream([&a]() { return std::to_string(a++); }) 
				| get(SIZE) 
				//| get(SIZE)
				//| get(SIZE)
				| skip(100) 
				| skip(100)
				| skip(100)
				| sum());
			cout << "Time: " << diffFromNow(start) << " Stream" << endl;
		}

		{
			using namespace fast_stream;
			using namespace fast_stream::operators;

			auto start = getCurrentTime();
			int a = 0;
			sum3 = std::move(buildStream([&a]() { return std::to_string(a++); })
				| get(SIZE) 
				//| get(SIZE)
				//| get(SIZE)
				| skip(100) 
				| skip(100)
				| skip(100)
				| sum());
			cout << "Time: " << diffFromNow(start) << " Fast Stream" << endl;
		}

		//ASSERT_EQ(sum1, sum2);
		//ASSERT_EQ(sum3, sum2);
		ASSERT_FALSE(true);
	}

	// Results: (Windows)
	// 6% deceleration (by fast_stream)
	TEST(Benchmark_stream_vs_fast_stream, DISABLED_filter) {
		const size_t SIZE = static_cast<size_t>(1e7);

		{
			auto start = getCurrentTime();
			int sum = 0;
			for (size_t i = 0; i < SIZE; i++)
				if (i % 2 == 0)
					sum += i;
			cout << "Time: " << diffFromNow(start) << " Simple" << endl;
		}

		{
			using namespace stream;
			using namespace stream::operators;

			auto start = getCurrentTime();
			int a = 0;
			buildStream([&a]() { return (a++); }) | get(SIZE)
				| filter([](auto l) { return l % 2 == 0; }) | sum();
			cout << "Time: " << diffFromNow(start) << " Stream" << endl;
		}

		{
			using namespace fast_stream;
			using namespace fast_stream::operators;

			auto sum1 = 
				buildStream(1, 2, 3, 4, 5, 6) 
				| filter([](auto l) { return l % 2 == 0; }) | sum();
			ASSERT_EQ(sum1, 12);

			auto start = getCurrentTime();
			int a = 0;
			buildStream([&a]() { return (a++); }) | get(SIZE)
				| filter([](auto l) { return l % 2 == 0; }) | sum();
			cout << "Time: " << diffFromNow(start) << " Fast Stream" << endl;
		}

		ASSERT_FALSE(true);
	}

}

