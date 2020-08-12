#include <gtest/gtest.h>

#include <string>
#include <chrono>
#include <stdlib.h>

#include "stream/stream.h"
#include "stream/short_stream/stream.h"
#include "stream/fast_stream/stream.h"

namespace stream_benchmarks {

	using namespace lipaboy_lib;

	using std::string;
	using std::chrono::steady_clock;

	decltype(auto) getCurrentTime() { return steady_clock::now(); }
	template <class ratio = std::chrono::milliseconds>
	decltype(auto) diffFromNow(steady_clock::time_point from) { 
		return std::chrono::duration_cast<ratio>(steady_clock::now() - from).count(); 
	}

	// Results: (Windows, AMD Ryzen 5 3500U 2019 year old, Release, 1e8)
	// 37% boost (50 simple, 2640 stream, 1667 fast_stream)
    // Results: (Linux, Intel B960 2010 years old, Release, 1e8)
    // 0% boost (97 simple, 2717 stream, 2751 fast_stream)
    TEST(Benchmark_stream_vs_fast_stream, DISABLED_skip) {
        const size_t SIZE = static_cast<size_t>(1e8);
		int sum1 = 0;
		int sum3 = 0;
		{
			auto start = getCurrentTime();
			for (size_t i = 0; i < SIZE; i++)
				if (i >= 100)
					sum1 += int(i);
			cout << "Time: " << diffFromNow(start) << " Simple" << endl;
		}

		{
			using namespace stream_space;
			using namespace stream_space::operators;

			auto start = getCurrentTime();
			int a = 0;
			Stream([&a]() { return a++; }) 
				| get(SIZE) 
				| skip(100) 
                | sum<>();
			cout << "Time: " << diffFromNow(start) << " Stream" << endl;
		}

		{
			using namespace fast_stream;
			using namespace fast_stream::operators;

			auto start = getCurrentTime();
			int a = 0;
			sum3 = Stream([&a]() { return a++; }) 
				| get(SIZE) 
				| skip(100) 
                | sum();
			cout << "Time: " << diffFromNow(start) << " Fast Stream" << endl;
		}


        ASSERT_EQ(sum1, sum3);
#ifdef WIN32
        system("pause");
#endif

		ASSERT_FALSE(true);
	}

	// Results: (Windows, AMD Ryzen 5 3500U 2019 year old, Release, 5e7)
	// 13% boost (6561 simple, 6343 stream, 5535 fast_stream) and 16% acceleration by using stream
    // Results: (Linux, Intel B960 2010 years old, Release, 1e7)
    // 0% boost (1936 simple, 2307 stream, 2300 fast_stream)
    TEST(Benchmark_stream_vs_fast_stream, DISABLED_skip_string) {
        const size_t SIZE = static_cast<size_t>(5e7);
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
			using namespace stream_space;
			using namespace stream_space::operators;

			auto start = getCurrentTime();
			int a = 0;
            sum2 = Stream([&a]() { return std::to_string(a++); })
				| get(SIZE) 
				| skip(100)
                | sum<>();
			cout << "Time: " << diffFromNow(start) << " Stream" << endl;
		}

		{
			using namespace fast_stream;
			using namespace fast_stream::operators;

			auto start = getCurrentTime();
			int a = 0;
            sum3 = Stream([&a]() { return std::to_string(a++); })
				| get(SIZE) 
				| skip(100)
                | sum();
			cout << "Time: " << diffFromNow(start) << " Fast Stream" << endl;
		}

#ifdef WIN32
        system("pause");
#endif

		//ASSERT_EQ(sum1, sum2);
		//ASSERT_EQ(sum3, sum2);
		ASSERT_FALSE(true);
	}

	// Results: (Windows, AMD Ryzen 5 3500U 2019 year old, Release, 1e8)
	// (2312 simple, 5214 stream, 5480 fast_stream, 5001 short_stream) and 56% deceleration
	// (4096 simple, 13519 stream, 13808 fast_stream, 12250 short_stream) and 70% deceleration
    // Results: (Linux, Intel B960 2010 years old, Release, 1e8)
    // 23% boost, deceleration by using stream 30%
    // (1347 simple, 5698 stream, 4358 fast_stream, 4681 short_stream)
    TEST(Benchmark_stream_vs_fast_stream, DISABLED_filter) {
        const size_t SIZE = static_cast<size_t>(2e8);
        auto filterFunc = [] (int) { return rand() % 2 == 0; };

		{
			auto start = getCurrentTime();
			int sum = 0;
			for (size_t i = 0; i < SIZE; i++)
                if (filterFunc(int(i)))
					sum += int(i);
			cout << "Time: " << diffFromNow(start) << " Simple" << endl;
		}

		{
			using namespace stream_space;
			using namespace stream_space::operators;

			auto start = getCurrentTime();
			int a = 0;
			Stream([&a]() { return (a++); }) | get(SIZE)
                | filter([&filterFunc](auto l) { return filterFunc(l); })
                | sum<>();
			cout << "Time: " << diffFromNow(start) << " Stream" << endl;
		}

		{
			using namespace fast_stream;
			using namespace fast_stream::operators;

			auto sum1 = 
				Stream(1, 2, 3, 4, 5, 6) 
                | filter([](auto l) { return l % 2 == 0; }) | sum();
			ASSERT_EQ(sum1, 12);

			auto start = getCurrentTime();
			int a = 0;
			Stream([&a]() { return (a++); }) | get(SIZE)
				| filter([filterFunc](auto l) { return filterFunc(l); }) | sum();
			cout << "Time: " << diffFromNow(start) << " Fast Stream" << endl;
		}

		{
			using namespace short_stream;
			using namespace short_stream::operators;

			auto stream =
				(buildShortStream(1, 2, 3, 4, 5, 6)
                    | filter([](auto l) -> bool { return l % 2 == 0; }));
			auto sum1 = stream | sum();
			ASSERT_EQ(sum1.value(), 12);

			auto start = getCurrentTime();
			int a = 0;
			buildShortStream([&a]() { return (a++); }) | get(SIZE)
				| filter([filterFunc](auto l) { return filterFunc(l); }) | sum();
			cout << "Time: " << diffFromNow(start) << " Short Stream" << endl;
		}

#ifdef WIN32
        system("pause");
#endif

		ASSERT_FALSE(true);
	}

	// Results: (Windows, AMD Ryzen 5 3500U 2019 year old, Release, 5e7)
	// (7786 simple, 11276 stream, 10465 fast_stream, 11579 short_stream) and 31% deceleration
    // Results: (Linux, Intel B960 2010 years old, Release, 1e7)
    // 0% boost, deceleration by using stream 0%
    // (3005 simple, 3064 stream, 3016 fast_stream, 3175 short_stream)
    TEST(Benchmark_stream_vs_fast_stream, DISABLED_filter_string) {
        const size_t SIZE = static_cast<size_t>(5e7);

		{
			auto start = getCurrentTime();
			string sum = "";
			for (size_t i = 0; i < SIZE; i++)
				if (std::stoi(std::to_string(i)) % 2 == 0)
					sum += std::to_string(i);
			cout << "Time: " << diffFromNow(start) << " Simple" << endl;
		}

		{
			using namespace stream_space;
			using namespace stream_space::operators;

			auto start = getCurrentTime();
			int a = 0;
			Stream([&a]() { return std::to_string(a++); }) | get(SIZE)
                | filter([](auto& l) { return std::stoi(l) % 2 == 0; })
                | sum<>();
			cout << "Time: " << diffFromNow(start) << " Stream" << endl;
		}

		{
			using namespace fast_stream;
			using namespace fast_stream::operators;

			auto start = getCurrentTime();
			int a = 0;
			Stream([&a]() { return std::to_string(a++); }) | get(SIZE)
				| filter([](auto& l) { return std::stoi(l) % 2 == 0; }) | sum();
			cout << "Time: " << diffFromNow(start) << " Fast Stream" << endl;
		}

		{
			using namespace short_stream;
			using namespace short_stream::operators;

			auto start = getCurrentTime();
			int a = 0;
			buildShortStream([&a]() { return std::to_string(a++); }) | get(SIZE)
				| filter([](auto& l) { return std::stoi(l) % 2 == 0; }) | sum();
			cout << "Time: " << diffFromNow(start) << " Short Stream" << endl;
		}

#ifdef WIN32
        system("pause");
#endif

		ASSERT_FALSE(true);
	}

}

