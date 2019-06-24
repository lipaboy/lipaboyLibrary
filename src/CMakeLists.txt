cmake_minimum_required(VERSION 2.8.2)
project(Lipaboy_Library_Source)

set(LIB_LIPABOY_LIB_SOURCE_FILES
    #lipaboy_test.cpp
    #lipaboy_test.h

    # Common Intefaces
    common_interfaces/algebra.h
    common_interfaces/comparable.h
    common_interfaces/containable.h
    common_interfaces/either_comparable.h

    # Containers
    containers/bit_vector.h

    # Extra tools
    extra_tools/extra_tools.h
    extra_tools/initializer_list_iterator.h
    extra_tools/maths_tools.h
    extra_tools/producing_iterator.h

    # HashMap
    hash_map/forward_list_storaged_size.h
    hash_map/hash_map.h

    # Intervals
    intervals/cubesegment.h
    intervals/cutoffborders.h
    intervals/interval.h
    intervals/rectsegment.h
    intervals/segment.h

    # Long Numbers
    long_numbers/long_number.h

    # Stream
    stream/stream_extended.h
    stream/stream_basic.h
    stream/stream.h
    stream/operators/distinct.h
    stream/operators/filter.h
    stream/operators/get.h
    stream/operators/group_by_vector.h
    stream/operators/map.h
    stream/operators/nth.h
    stream/operators/operators.h
    stream/operators/print_to.h
    stream/operators/reduce.h
    stream/operators/skip.h
    stream/operators/sum.h
    stream/operators/to_vector.h
    stream/operators/tools.h
    stream/operators/ungroup_by_bit.h
	stream/operators/group.h

    # Precision Number
    maths/fixed_precision_number.h
    maths/precision_number_can_throw.h
    maths/precision_number.h

    # Maths
    maths/matrix4x4.cpp
    maths/matrix4x4.h
    maths/line2d.h
    maths/point2d.h
    maths/rotateoperator.h
    maths/rotateoperator.cpp
    maths/vector2d.h
    maths/vector3d.h
    maths/vector4d.h
    maths/vector4d.cpp
)

# Declare the library
add_library(LIPABOY_LIB STATIC
    ${LIB_LIPABOY_LIB_SOURCE_FILES}
)

if(WIN32)
	add_definitions( -DBOOST_ALL_DYN_LINK )  #If not VS will give linking errors of redefinitions
	set(Boost_USE_STATIC_LIBS OFF )
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)
    find_package(Boost COMPONENTS thread chrono regex REQUIRED )

    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
endif(WIN32)

if(WIN32)
    TARGET_LINK_LIBRARIES( LIPABOY_LIB ${Boost_LIBRARIES} )
endif(WIN32)


# Specify here the include directories exported
# by this library
target_include_directories(LIPABOY_LIB PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)


