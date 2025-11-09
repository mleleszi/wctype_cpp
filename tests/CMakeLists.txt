include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(wctype_tests
    iswalpha_test.cpp
    iswdigit_test.cpp
    iswpunct_test.cpp
    iswalnum_test.cpp
    iswblank_test.cpp
    iswgraph_test.cpp
    iswlower_test.cpp
    iswspace_test.cpp
)
target_link_libraries(wctype_tests PRIVATE
    GTest::gtest_main
    mywctype
)

include(GoogleTest)
gtest_discover_tests(wctype_tests)

add_custom_target(check-wctype
        COMMAND wctype_tests
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running tests..."
)
