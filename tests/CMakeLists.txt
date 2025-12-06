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
        iswupper_test.cpp
        iswspace_test.cpp
        iswprint_test.cpp
        iswcntrl_test.cpp
        iswxdigit_test.cpp
        iswctype_test.cpp
        wctype_test.cpp
        towupper_test.cpp
        towlower_test.cpp
)
target_link_libraries(wctype_tests PRIVATE
        GTest::gtest_main
        mywctype
        v8_unicode
)

include(GoogleTest)
gtest_discover_tests(wctype_tests)

add_custom_target(check-all
        COMMAND wctype_tests
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running all tests..."
)

# Individual test targets
add_custom_target(check-iswalpha
        COMMAND wctype_tests --gtest_filter="AlphaTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswalpha tests..."
)

add_custom_target(check-iswdigit
        COMMAND wctype_tests --gtest_filter="DigitTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswdigit tests..."
)

add_custom_target(check-iswpunct
        COMMAND wctype_tests --gtest_filter="PunctTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswpunct tests..."
)

add_custom_target(check-iswalnum
        COMMAND wctype_tests --gtest_filter="AlnumTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswalnum tests..."
)

add_custom_target(check-iswblank
        COMMAND wctype_tests --gtest_filter="BlankTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswblank tests..."
)

add_custom_target(check-iswgraph
        COMMAND wctype_tests --gtest_filter="GraphTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswgraph tests..."
)

add_custom_target(check-iswlower
        COMMAND wctype_tests --gtest_filter="LowerTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswlower tests..."
)

add_custom_target(check-iswupper
        COMMAND wctype_tests --gtest_filter="UpperTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswupper tests..."
)

add_custom_target(check-iswspace
        COMMAND wctype_tests --gtest_filter="SpaceTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswspace tests..."
)

add_custom_target(check-iswprint
        COMMAND wctype_tests --gtest_filter="PrintTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswprint tests..."
)

add_custom_target(check-iswcntrl
        COMMAND wctype_tests --gtest_filter="CntrlTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswcntrl tests..."
)

add_custom_target(check-iswxdigit
        COMMAND wctype_tests --gtest_filter="XdigitTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswxdigit tests..."
)

add_custom_target(check-iswctype
        COMMAND wctype_tests --gtest_filter="IswctypeTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running iswctype tests..."
)

add_custom_target(check-wctype
        COMMAND wctype_tests --gtest_filter="WctypeMapTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running wctype mapping tests..."
)

add_custom_target(check-towupper
        COMMAND wctype_tests --gtest_filter="TowupperTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running towupper tests..."
)

add_custom_target(check-towlower
        COMMAND wctype_tests --gtest_filter="TowlowerTest*"
        DEPENDS wctype_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running towlower tests..."
)
