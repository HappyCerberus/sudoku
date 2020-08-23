# automatically enable catch2 to generate ctest targets
if (CONAN_CATCH2_ROOT_DEBUG)
    include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else ()
    include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif ()

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC CONAN_PKG::catch2)
target_link_libraries(catch_main PRIVATE project_options)

add_executable(unique_block_tests UniqueBlockTest.cpp)
target_link_libraries(unique_block_tests PRIVATE sudoku_lib project_warnings project_options
        catch_main)        

add_executable(sudoku_tests SudokuTest.cpp)
target_link_libraries(sudoku_tests PRIVATE sudoku_lib project_warnings project_options
        catch_main)

add_executable(solver_tests SmartSolverTest.cpp)
target_link_libraries(solver_tests PRIVATE sudoku_lib project_warnings project_options
        catch_main)

add_executable(sudoku_algorithms_tests SudokuAlgorithmsTest.cpp)
target_link_libraries(sudoku_algorithms_tests PRIVATE core sudoku_algorithms project_warnings project_options
        catch_main)

# automatically discover tests that are defined in catch based test files you
# can modify the unittests. TEST_PREFIX to whatever you want, or use different
# for different binaries
catch_discover_tests(
        sudoku_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)

# automatically discover tests that are defined in catch based test files you
# can modify the unittests. TEST_PREFIX to whatever you want, or use different
# for different binaries
catch_discover_tests(
        unique_block_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)

# automatically discover tests that are defined in catch based test files you
# can modify the unittests. TEST_PREFIX to whatever you want, or use different
# for different binaries
catch_discover_tests(
        solver_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)



# Disable the constexpr portion of the test, and build again this allows us to have an executable that we can debug when
# things go wrong with the constexpr testing
add_executable(relaxed_constexpr_tests ConstexprTests.cpp)
target_link_libraries(relaxed_constexpr_tests PRIVATE core project_options project_warnings catch_main)
target_compile_definitions(relaxed_constexpr_tests PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

catch_discover_tests(
  relaxed_constexpr_tests
  TEST_PREFIX
  "relaxed_constexpr."
  EXTRA_ARGS
  -s
  --reporter=xml
  --out=relaxed_constexpr.xml)

# Add a file containing a set of constexpr tests
add_executable(constexpr_tests ConstexprTests.cpp)
target_link_libraries(constexpr_tests PRIVATE core project_options project_warnings catch_main)

catch_discover_tests(
  constexpr_tests
  TEST_PREFIX
  "constexpr."
  EXTRA_ARGS
  -s
  --reporter=xml
  --out=constexpr.xml)


