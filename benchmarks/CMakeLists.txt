include(FetchContent)
FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.1
        GIT_SHALLOW TRUE
        FIND_PACKAGE_ARGS NAMES benchmark
)

set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)

FetchContent_MakeAvailable(googlebenchmark)

add_executable(towupper_benchmark
        towupper_benchmark.cpp
)

target_link_libraries(towupper_benchmark PRIVATE
        benchmark::benchmark
        mywctype
)

target_compile_definitions(towupper_benchmark PRIVATE
        BENCHMARK_LOCALE="C.UTF-8"
)

add_executable(towlower_benchmark
        towlower_benchmark.cpp
)

target_link_libraries(towlower_benchmark PRIVATE
        benchmark::benchmark
        mywctype
)

target_compile_definitions(towlower_benchmark PRIVATE
        BENCHMARK_LOCALE="C.UTF-8"
)

add_executable(classification_benchmark
        classification_benchmark.cpp
)

target_link_libraries(classification_benchmark PRIVATE
        benchmark::benchmark
        mywctype
)

target_compile_definitions(classification_benchmark PRIVATE
        BENCHMARK_LOCALE="C.UTF-8"
)

add_custom_target(bench-towupper
        COMMAND towupper_benchmark
        DEPENDS towupper_benchmark
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running towupper benchmarks..."
)

add_custom_target(bench-towlower
        COMMAND towlower_benchmark
        DEPENDS towlower_benchmark
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running towlower benchmarks..."
)

add_custom_target(bench-classification
        COMMAND classification_benchmark
        DEPENDS classification_benchmark
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running classification benchmarks..."
)

add_custom_target(bench-all
        COMMAND towupper_benchmark
        COMMAND towlower_benchmark
        COMMAND classification_benchmark
        DEPENDS towupper_benchmark towlower_benchmark classification_benchmark
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running all benchmarks..."
)