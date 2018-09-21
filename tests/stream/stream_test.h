#pragma once

#include <gtest/gtest.h>

//#define STREAM_V1_TESTS_RUN

#ifdef STREAM_V1_TESTS_RUN
#include "stream/stream_v1/stream.h"
#else
#include "stream/stream.h"
#endif

#include "extra_tools/extra_tools_tests.h"

#include <fstream>

namespace stream_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unique_ptr;

using namespace lipaboy_lib;


#ifdef STREAM_V1_TESTS_RUN
using stream_v1_space::Stream;
using stream_v1_space::StreamOfOutsideIterators;
using namespace lipaboy_lib::stream_v1_space::operators_space;
#else
using stream_space::Stream;
using stream_space::StreamOfOutsideIterators;
using namespace lipaboy_lib::stream_space::operators_space;
#endif

using lipaboy_lib_tests::Noisy;

class PrepareStreamTest : public ::testing::Test  {
public:
    using ElemType = int;
    using Container = vector<ElemType>;
    using StreamType = StreamOfOutsideIterators<typename Container::iterator>;
    using StreamTypePtr = unique_ptr<StreamType>;

protected:
    void SetUp() {
        pOutsideContainer = std::unique_ptr<Container>(new Container({ 1, 2, 3, 4, 5 }));

        //---------------File Stream Init------------//

        std::ofstream outFile;
        filename = "temp.stream.lol";
        outFile.open(filename, std::ios::out | std::ios::trunc);
        fileData = "lol kek cheburek";
        outFile << fileData;
        outFile.close();
    }
    void TearDown() {
        std::remove(filename.c_str());
    }

	typename Container::iterator begin() { return pOutsideContainer->begin(); }
	typename Container::iterator end() { return pOutsideContainer->end(); }

protected:
    unique_ptr<Container> pOutsideContainer;
    string filename;
    string fileData;
};


//class InfiniteStreamTest : public ::testing::Test {
//public:
//	using ElemType = int;
//	using Container = vector<ElemType>;
//	using StreamType = stream_space::StreamOfGenerator<std::function<ElemType(void)> >;
//	using StreamTypePtr = unique_ptr<StreamType>;
//
//protected:
//	void SetUp() {
//		static int x = 0;
//		pStream = std::unique_ptr<StreamType>(stream_space::allocateStream([&a = x]() -> ElemType { return a++; }));
//	}
//	void TearDown() {
//	}
//
//protected:
//	StreamTypePtr pStream;
//};

} 


