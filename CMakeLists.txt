cmake_minimum_required(VERSION 3.31)
project(wctype_cpp)

set(CMAKE_CXX_STANDARD 23)

# Build the generator as a separate executable (not included in final binary)
add_executable(generator generator.cpp generator.h)

set(GENERATED_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_properties.h
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_level1.inc
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_level2.inc
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_table.h
)

# Copy unicodedata directory to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/unicodedata
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Custom command for running the generator at build time
add_custom_command(
        OUTPUT ${GENERATED_FILES}
        COMMAND generator
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS generator ${CMAKE_CURRENT_SOURCE_DIR}/unicodedata/UnicodeData.txt
        COMMENT "Generating wctype lookup tables..."
)

add_custom_target(generate_tables DEPENDS ${GENERATED_FILES})

add_executable(wctype_cpp main.cpp)

add_dependencies(wctype_cpp generate_tables)

target_include_directories(wctype_cpp PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
