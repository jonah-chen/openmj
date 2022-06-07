cmake_minimum_required(VERSION 3.23.2 FATAL_ERROR)
project(OpenMJ)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
# find_package(OpenMP REQUIRED)

set(src_dir ${CMAKE_CURRENT_LIST_DIR}/src)
set(test_dir ${CMAKE_CURRENT_LIST_DIR}/test)
set(extern_dir ${CMAKE_CURRENT_LIST_DIR}/src/extern)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${src_dir})


message(STATUS "Building Libraries")
add_library(Mahjong STATIC ${src_dir}/core/mahjong/mahjong.cc
                        ${src_dir}/core/mahjong/scoring.cc)
add_library(tomohxx_Shanten STATIC ${extern_dir}/shanten-number/calsht.cpp)


set(libs Mahjong tomohxx_Shanten)


add_executable(Benchmark ${test_dir}/bench.cc)
target_link_libraries(Benchmark ${libs})

enable_testing()

function(makeTest test_rp test_name)
add_executable(${test_name} ${test_dir}/${test_rp}/${test_name}.cc)
target_link_libraries(${test_name} ${libs})
add_test(${test_name} ${test_name})
endfunction()

message(STATUS "Building Tests")

makeTest(core/mahjong tile)
makeTest(core/mahjong meld)
makeTest(core/mahjong hand)
makeTest(core/mahjong scoring)

makeTest(core/utils stack_allocator)
