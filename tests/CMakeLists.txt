set(MODULE_NAME Lipaboy_Library)

find_package(Threads REQUIRED)

# TODO: unable to load gtest after each cmake running (only once)

include(ExternalProject)
ExternalProject_Add(
        gtest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        INSTALL_COMMAND ""
        CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
)

# Get GTest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir binary_dir)

#message(STATUS "binary_dir=${CMAKE_CURRENT_SOURCE_DIR}")
##set(dir ${CMAKE_CURRENT_SOURCE_DIR}/../../build*LipaboyLibrary*Debug*)
#set(dir ~/Projects/build-LipaboyLibrary-Desktop_Qt_5_10_1_GCC_7_64bit-Debug)
##set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
##set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
#message(STATUS "binary_dir=${dir}")
#add_subdirectory(${dir}/tests/gtest)
#enable_testing()
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)
add_dependencies(libgtest Lipaboy_Library_Source)

# Set libgtest properties
set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)

# Set libgmock properties
set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        )

include_directories(
        "${CMAKE_SOURCE_DIR}/src"
        "${source_dir}/googletest/include"
        "${source_dir}/googlemock/include")

set(TEST_SOURCE
    tests.cpp
    stream_tests.cpp
    stream_test.h
    )
add_executable(${MODULE_NAME}_test ${TEST_SOURCE})

target_link_libraries(
        ${MODULE_NAME}_test
        libgmock
        libgtest
)

install(TARGETS ${MODULE_NAME}_test DESTINATION bin)

add_test(NAME ${MODULE_NAME}_test COMMAND ${MODULE_NAME}_test)
