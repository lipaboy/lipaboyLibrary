#ifndef HASH_MAP_TEST_H
#define HASH_MAP_TEST_H

#include "gtest/gtest.h"
#include "stream/stream.h"

#include "extra_tools/extra_tools_tests.h"

#include <fstream>

namespace stream_tests {

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::unique_ptr;

using stream_space::Stream;
using stream_space::IsOutsideIteratorsRefer;

using lipaboy_lib_tests::Noisy;

class OutsideItersStreamTest : public ::testing::Test  {
public:
    using ElemType = int;
    using Container = vector<ElemType>;
    using StreamType = Stream<IsOutsideIteratorsRefer, typename Container::iterator>;
    using StreamTypePtr = unique_ptr<StreamType>;

protected:
    void SetUp() {
        pOutsideContainer = std::unique_ptr<Container>(new Container({ 1, 2, 3, 4, 5 }));
        pStream = std::unique_ptr<StreamType>(stream_space::makeStream(pOutsideContainer->begin(),
                                                                      pOutsideContainer->end()));
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

protected:
    StreamTypePtr pStream;
    unique_ptr<Container> pOutsideContainer;
    string filename;
    string fileData;
};

}

#endif // HASH_MAP_TEST_H
