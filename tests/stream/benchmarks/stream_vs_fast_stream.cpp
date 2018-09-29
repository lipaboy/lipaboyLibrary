#include <gtest/gtest.h>

#include "stream/stream.h"

namespace stream_benchmarks {

	using namespace lipaboy_lib;

	TEST(Benchmark, DISABLED_stream_vs_fast_stream) {
		{
			using namespace stream;
			using namespace stream::operators;

			auto start = clock();
			buildStream([]() { return rand(); }) | skip(100) | get(1e7) | sum();
			cout << "Time: " << clock() - start << endl;
		}

		{
			using namespace fast_stream;
			using namespace fast_stream::operators;

			auto start = clock();
			buildStream([]() { return rand(); }) | skip(100)
				| get(1e7) | sum();
			cout << "Time: " << clock() - start << endl;
		}

		//ASSERT_FALSE(true);
	}

}