cmake_minimum_required(VERSION 3.5.1)
project(Lipaboy_Library)

include_directories(
        "${CMAKE_SOURCE_DIR}/src"
        "${source_dir}/googletest/include"
        "${source_dir}/googlemock/include")

include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()

enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-std=c++17")
# TODO: remove this line
#enable_cxx_compiler_flag_if_supported("-fpermissive")

add_subdirectory(src)
add_subdirectory(tests)
