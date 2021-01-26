set(MODULE_NAME Lipaboy_Library)

include_directories("${CMAKE_SOURCE_DIR}/tests")
include_directories("${CMAKE_SOURCE_DIR}/src")

set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG "_DEBUG")

set(TEST_SOURCE
    tests.cpp

	# Algebra
    common_interfaces/algebra_tests.cpp
    intervals/interval_tests.cpp

	# Long Numbers
    long_numbers/long_unsigned_tests.cpp
    long_numbers/long_numbers_tests.cpp
    long_numbers/big_integer_tests.cpp
    long_numbers/long_numbers_benchmark_tests.cpp

	# Stream
    stream/stream_tests.cpp
    stream/stream_test.h
    stream/benchmarks/stream_vs_fast_stream.cpp

    stream/tupled_stream_tests.cpp
    stream/paired_stream_tests.cpp
    stream/nop_tests.cpp
    stream/nth_tests.cpp
    stream/filter_tests.cpp
    stream/reduce_tests.cpp
    stream/max_min_tests.cpp
    stream/split_tests.cpp
    "stream/cast_tests.cpp"

	# HashMap
    hash_map/forward_list_tests.cpp
    hash_map/hash_map_test.h
    hash_map/hash_map_tests.cpp

	# Extra tools
    extra_tools/producing_iterator_tests.cpp
    extra_tools/initializer_list_iterator_tests.cpp
    extra_tools/maths_tools_tests.cpp
    extra_tools/extra_tools_tests.cpp
    extra_tools/extra_tools_tests.h

	# Maths
    maths/fixed_precision_number_tests.cpp

    # Container
    containers/bit_vector_benchmark.cpp
)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_executable(${MODULE_NAME}_tests ${TEST_SOURCE})

target_link_libraries(${MODULE_NAME}_tests 
	LIPABOY_LIB 
	gtest 
        gmock_main)

add_test(NAME ${MODULE_NAME}_tests COMMAND ${MODULE_NAME}_tests)
