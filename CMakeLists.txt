# (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)
cmake_minimum_required(VERSION 3.10)

project(sudoku CXX)
#set(CMAKE_CXX_STANDARD 17)
include(cmake/StandardProjectSettings.cmake)
# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)
# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# Setup Google Test
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif ()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif ()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
include_directories("${gtest_SOURCE_DIR}/include")
include_directories("${gmock_SOURCE_DIR}/include")

set(CMAKE_CXX_FLAGS "-O3")
#set(CMAKE_CXX_FLAGS "-O0 -ggdb3")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(SudokuSquareTest SudokuSquareTest.cpp SudokuSquare.h SudokuSquare.cpp)
target_link_libraries(SudokuSquareTest gtest_main)
add_test(NAME SudokuSquareTest COMMAND SudokuSquareTest)

add_executable(BlockCheckerTest BlockCheckerTest.cpp BlockChecker.cpp BlockChecker.h)
target_link_libraries(BlockCheckerTest gtest_main gmock)
add_test(NAME BlockCheckerTest COMMAND BlockCheckerTest)

add_executable(SudokuTest SudokuTest.cpp Sudoku.cpp Sudoku.h SudokuSquare.h SudokuSquare.cpp BlockChecker.cpp BlockChecker.h)
target_link_libraries(SudokuTest gtest_main)
add_test(NAME SudokuTest COMMAND SudokuTest)

add_executable(sudoku Sudoku.cpp Sudoku.h SudokuSquare.h SudokuSquare.cpp BlockChecker.cpp BlockChecker.h main.cpp)