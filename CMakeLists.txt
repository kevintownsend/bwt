cmake_minimum_required(VERSION 3.16)

project(bwt LANGUAGES CXX)

include(CTest)
include(GNUInstallDirs)

option(BWT_BUILD_EXAMPLES "Build CLI example and benchmark executables" ON)
option(BWT_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

add_library(bwt INTERFACE)
add_library(bwt::bwt ALIAS bwt)

target_include_directories(
  bwt
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(bwt INTERFACE cxx_std_17)

function(bwt_apply_warnings target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE /W4)
    if(BWT_WARNINGS_AS_ERRORS)
      target_compile_options(${target_name} PRIVATE /WX)
    endif()
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    if(BWT_WARNINGS_AS_ERRORS)
      target_compile_options(${target_name} PRIVATE -Werror)
    endif()
  endif()
endfunction()

if(BWT_BUILD_EXAMPLES)
  add_executable(bwt_cli examples/bwt_cli.cc)
  target_link_libraries(bwt_cli PRIVATE bwt::bwt)
  bwt_apply_warnings(bwt_cli)

  add_library(rand_int_benchmark STATIC benchmark/rand_int_benchmark.cc)
  target_link_libraries(rand_int_benchmark PUBLIC bwt::bwt)
  target_include_directories(rand_int_benchmark
                             PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
  bwt_apply_warnings(rand_int_benchmark)

  add_executable(rand_int_benchmark_bin benchmark/rand_int_benchmark_bin.cc)
  target_link_libraries(rand_int_benchmark_bin PRIVATE rand_int_benchmark)
  bwt_apply_warnings(rand_int_benchmark_bin)
endif()

if(BUILD_TESTING)
  find_package(GTest QUIET)
  if(NOT GTest_FOUND)
    include(FetchContent)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz)
    FetchContent_MakeAvailable(googletest)
  endif()

  include(GoogleTest)

  add_executable(bwt_test bwt_test.cc)
  target_link_libraries(bwt_test PRIVATE bwt::bwt GTest::gtest_main)
  bwt_apply_warnings(bwt_test)
  gtest_discover_tests(bwt_test)

  add_executable(rand_int_benchmark_test
                 benchmark/rand_int_benchmark.cc
                 benchmark/rand_int_benchmark_test.cc)
  target_link_libraries(rand_int_benchmark_test PRIVATE bwt::bwt
                                                        GTest::gtest_main)
  target_include_directories(rand_int_benchmark_test
                             PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  bwt_apply_warnings(rand_int_benchmark_test)
  gtest_discover_tests(rand_int_benchmark_test)

  if(BWT_BUILD_EXAMPLES)
    add_test(NAME bwt_cli_smoke COMMAND bwt_cli gataca)
    set_tests_properties(bwt_cli_smoke
                         PROPERTIES PASS_REGULAR_EXPRESSION "actga\\$a[\r\n]+gataca")
  endif()
endif()

install(FILES bwt.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
