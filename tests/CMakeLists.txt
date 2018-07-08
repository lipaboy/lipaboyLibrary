set(MODULE_NAME Lipaboy_Library)

include_directories("${CMAKE_SOURCE_DIR}/src")

set(TEST_SOURCE
    tests.cpp

    stream/stream_tests.cpp
    stream/stream_test.h

    hash_map/forward_list_tests.cpp
    hash_map/hash_map_test.h
    hash_map/hash_map_tests.cpp
    )

add_executable(${MODULE_NAME}_tests ${TEST_SOURCE})
target_link_libraries(${MODULE_NAME}_tests gtest gmock_main)

set_property(TARGET ${MODULE_NAME}_tests PROPERTY CXX_STANDARD 17)
set_property(TARGET ${MODULE_NAME}_tests PROPERTY CXX_STANDARD_REQUIRED ON)

add_test(NAME ${MODULE_NAME}_tests COMMAND ${MODULE_NAME}_tests)
