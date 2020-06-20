SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_library(sudoku_lib Sudoku.cpp Sudoku.h Square.h Square.cpp BlockChecker.cpp BlockChecker.h SolveStats.cpp
        SolveStats.h SmartSolver.cpp SmartSolver.h)
target_link_libraries(sudoku_lib project_options project_warnings)

add_executable(sudoku main.cpp)
target_link_libraries(sudoku sudoku_lib project_options project_warnings)


find_package(benchmark REQUIRED)
add_executable(benchmarks benchmarks.cpp)
target_link_libraries(benchmarks sudoku_lib project_options project_warnings benchmark::benchmark)