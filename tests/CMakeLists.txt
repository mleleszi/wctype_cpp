include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
        GIT_SHALLOW TRUE
        FIND_PACKAGE_ARGS NAMES GTest
)

# Try to use system-installed GTest first, fall back to fetch
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)

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
        iswxdigit_test.cpp
        iswctype_test.cpp
        wctype_test.cpp
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
