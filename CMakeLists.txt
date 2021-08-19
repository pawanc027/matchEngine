cmake_minimum_required(VERSION 3.10)

# set the project name
project(matchEngine LANGUAGES CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify Flags
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# I../includes
include_directories(inc)
include_directories(test)

# Add the executable
add_executable(matchEngine src/main.cpp src/matchEngine.cpp)
target_link_libraries(matchEngine pthread) 

install(TARGETS matchEngine DESTINATION bin)

set_target_properties( matchEngine 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)


enable_testing()

include(FindUnixCommands)

# Define a function to simplify adding tests
function(do_test target input result expected_result)
add_test(NAME test01 
         COMMAND sh -c "${target} < ${input} > ${result}" && ${CMAKE_COMMAND} -E compare_files "${result}" "${expected_result}")
endfunction(do_test)

# Tests with Valid Input
do_test("${CMAKE_CURRENT_SOURCE_DIR}/build/bin/matchEngine" "${CMAKE_CURRENT_SOURCE_DIR}/test/test01_input.txt"  "${CMAKE_CURRENT_SOURCE_DIR}/test/temp" "${CMAKE_CURRENT_SOURCE_DIR}/test/test01_expected_output.txt")
