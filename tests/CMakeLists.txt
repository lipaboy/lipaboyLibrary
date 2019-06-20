set(MODULE_NAME Lipaboy_Library)

include_directories("${CMAKE_SOURCE_DIR}/tests")
include_directories("${CMAKE_SOURCE_DIR}/src")

set(TEST_SOURCE
    tests.cpp

    common_interfaces/algebra_tests.cpp

    long_numbers/long_numbers_tests.cpp

    stream/stream_tests.cpp
    stream/stream_test.h
	stream/benchmarks/stream_vs_fast_stream.cpp

    hash_map/forward_list_tests.cpp
    hash_map/hash_map_test.h
    hash_map/hash_map_tests.cpp

    extra_tools/producing_iterator_tests.cpp
    extra_tools/initializer_list_iterator_tests.cpp
    extra_tools/maths_tools_tests.cpp
    extra_tools/extra_tools_tests.cpp
    extra_tools/extra_tools_tests.h

	maths/fixed_precision_number_tests.cpp
    )

add_executable(${MODULE_NAME}_tests ${TEST_SOURCE})
target_link_libraries(${MODULE_NAME}_tests 
	LIPABOY_LIB 
	gtest 
        gmock_main)

add_test(NAME ${MODULE_NAME}_tests COMMAND ${MODULE_NAME}_tests)
