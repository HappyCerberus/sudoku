SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(core)

add_library(sudoku_lib Sudoku.cpp Sudoku.h SolveStats.cpp
        SolveStats.h SmartSolver.cpp SmartSolver.h Progressbar.cpp Progressbar.h)
        #  KillerBlockChecker.cpp KillerBlockChecker.h SmallKillerBlockChecker.cpp SmallKillerBlockChecker.h
target_link_libraries(sudoku_lib core sudoku_algorithms project_options project_warnings)

add_executable(sudoku main.cpp)
target_link_libraries(sudoku sudoku_lib project_options project_warnings)

add_executable(benchmarks benchmarks.cpp)
target_link_libraries(benchmarks sudoku_lib project_options project_warnings CONAN_PKG::benchmark)