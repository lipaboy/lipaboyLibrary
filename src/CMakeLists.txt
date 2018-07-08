cmake_minimum_required(VERSION 2.8.2)
project(Lipaboy_Library_Source)

set (CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    lipaboy_test.cpp
    lipaboy_test.h

    # Stream
    functors.h
    stream_extended.h
    stream_base.h
    range.h
    stream.h

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

#add_executable(lipaboyLibRun ${SOURCE_FILES})
#set_target_properties(lipaboyLibRun PROPERTIES
#    CXX_STANDARD_REQUIRED 17)

