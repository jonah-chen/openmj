cmake_minimum_required(VERSION 3.23.2 FATAL_ERROR)
project(OpenMJ)
include(CheckLanguage)

set(src_dir ${CMAKE_CURRENT_LIST_DIR}/src)
set(test_dir ${CMAKE_CURRENT_LIST_DIR}/test)
set(extern_dir ${CMAKE_CURRENT_LIST_DIR}/src/extern)
set(example_dir ${CMAKE_CURRENT_LIST_DIR}/example)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -fno-rtti -DMJ_LOGGING=2 -w")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -fno-rtti -flto=auto -DNDEBUG -DMJ_LOGGING=0")

option(BUILD_BENCH "Build Benchmarks" ON)
option(BUILD_TESTS "Build Tests" ON)

check_language(CUDA)
if (CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    message(STATUS "CUDA found, will compile CUDA modules")
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_FLAGS_DEBUG "${CMAKE_CUDA_FLAGS_DEBUG} -G -Wall -Wextra")
    add_library(mj_pch OBJECT ${src_dir}/pch/gpu.cu)
    target_precompile_headers(mj_pch PUBLIC ${src_dir}/pch/gpu.hpp)
else()
    add_library(mj_pch OBJECT ${src_dir}/pch/cpu.cc)
    target_precompile_headers(mj_pch PUBLIC ${src_dir}/pch/cpu.hpp)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
if (BUILD_BENCH)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING "Benchmarks should be built in Release mode, but are being built in Debug mode. Results may be inaccurate.")
    endif()
    find_package(benchmark REQUIRED)
    set(bench_libs benchmark::benchmark)
endif()

# find_package(OpenMP REQUIRED)

# use precompiled headers


include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${src_dir})
include_directories(${glfw3_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})

message(STATUS "Building Libraries")

add_library(Mahjong STATIC ${src_dir}/core/mahjong/hand.cc)
target_precompile_headers(Mahjong REUSE_FROM mj_pch)

add_library(tomohxx_Shanten STATIC ${extern_dir}/shanten-number/calsht.cpp)
target_precompile_headers(tomohxx_Shanten REUSE_FROM mj_pch)

add_library(imgui_GLFW STATIC ${extern_dir}/imgui/imgui_impl_glfw.cpp
                          ${extern_dir}/imgui/imgui_impl_opengl3.cpp
                          ${extern_dir}/imgui/imgui.cpp
                          ${extern_dir}/imgui/imgui_tables.cpp
                          ${extern_dir}/imgui/imgui_draw.cpp
                          ${extern_dir}/imgui/imgui_widgets.cpp)
target_precompile_headers(imgui_GLFW REUSE_FROM mj_pch)
target_link_libraries(imgui_GLFW PUBLIC ${glfw3_LIBRARIES} ${CMAKE_DL_LIBS})

add_library(Engine STATIC ${src_dir}/core/engine/gui.cc ${extern_dir}/imgui/imgui_demo.cpp
                        ${src_dir}/core/engine/layerstack.cc
                        ${src_dir}/core/engine/window.cc
                        ${src_dir}/core/engine/app.cc
                        ${src_dir}/core/engine/texture.cc
                        ${src_dir}/core/engine/hand.cc)
target_precompile_headers(Engine REUSE_FROM mj_pch)
target_link_libraries(Engine PUBLIC glfw imgui_GLFW GLEW OpenGL)


set(libs Mahjong tomohxx_Shanten)


message(STATUS "Building Examples")
add_executable(gui_example ${example_dir}/gui.cc ${src_dir}/sims/selector.cc)
target_precompile_headers(gui_example REUSE_FROM mj_pch)
target_link_libraries(gui_example PUBLIC ${libs} Engine)

message(STATUS "Building Executables")
add_executable(Riichi ${src_dir}/sims/riichi_app.cc ${src_dir}/sims/riichi.cc)
target_precompile_headers(Riichi REUSE_FROM mj_pch)
target_link_libraries(Riichi PUBLIC ${libs} Engine pthread)



if (BUILD_BENCH)
    add_executable(Benchmark ${test_dir}/bench.cc)
    target_precompile_headers(Benchmark REUSE_FROM mj_pch)
    target_link_libraries(Benchmark ${libs} ${bench_libs})
endif()

if (BUILD_TESTS)
    enable_testing()

    function(makeTest test_rp test_name)
    add_executable(${test_name} ${test_dir}/${test_rp}/${test_name}.cc)
    target_precompile_headers(${test_name} REUSE_FROM mj_pch)
    target_link_libraries(${test_name} ${libs})
    add_test(${test_name} ${test_name})
    endfunction()

    function(makeTestCU test_rp test_name)
    add_executable(CU${test_name} ${test_dir}/${test_rp}/${test_name}.cu)
    target_precompile_headers(CU${test_name} REUSE_FROM mj_pch)
    target_link_libraries(CU${test_name} ${libs})
    add_test(CU${test_name} CU${test_name})
    endfunction()

    message(STATUS "Building Tests")

    makeTest(core/mahjong tile)
    makeTest(core/mahjong meld)
    makeTest(core/mahjong hand)
    makeTest(core/mahjong scoring_closed)
    makeTest(core/mahjong scoring_open)
    makeTest(core/mahjong rng)
    makeTest(cuda vector)
    makeTest(sims riichi)

    if (CMAKE_CUDA_COMPILER)
    makeTestCU(cuda vector)

    endif()
    # makeTest(core/utils stack_allocator)
endif()
