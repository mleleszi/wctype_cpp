# Build the generator as a separate executable (not included in final binary)
add_executable(generator generator.cpp generator.h)

set(GENERATED_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_properties.h
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_level1.inc
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_level2.inc
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_table.h

        # binary search conversions
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping.h
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_lower.inc
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_upper.inc

        # staged conversions
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_staged.h
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_lower_level1.inc
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_lower_level2.inc
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_upper_level1.inc
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_upper_level2.inc

        ${CMAKE_CURRENT_BINARY_DIR}/ht.h
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../unicodedata
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Custom command for running the generator at build time
add_custom_command(
        OUTPUT ${GENERATED_FILES}
        COMMAND generator
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_properties.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/classification/wctype_properties.h
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_level1.inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/classification/wctype_level1.inc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_level2.inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/classification/wctype_level2.inc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/wctype_table.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/classification/wctype_table.h

        # binary search
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/conversion/case_mapping.h
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_lower.inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/conversion/case_mapping_lower.inc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_upper.inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/conversion/case_mapping_upper.inc

        # staged
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_staged.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/conversion/case_mapping_staged.h
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_lower_level1_staged.inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/conversion/case_mapping_lower_level1_staged.inc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_lower_level2_staged.inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/conversion/case_mapping_lower_level2_staged.inc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_upper_level1_staged.inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/conversion/case_mapping_upper_level1_staged.inc
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/case_mapping_upper_level2_staged.inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated/conversion/case_mapping_upper_level2_staged.inc
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS generator ${CMAKE_CURRENT_SOURCE_DIR}/../unicodedata/UnicodeData.txt
        COMMENT "Generating wctype lookup tables..."
)

add_custom_target(generate_tables DEPENDS ${GENERATED_FILES})

# INTERFACE library (header-only)
add_library(mywctype INTERFACE)
target_include_directories(mywctype INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies(mywctype generate_tables)

# STATIC library for size measurement
add_library(mywctype_static STATIC wctype_tables.cpp)
target_include_directories(mywctype_static PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies(mywctype_static generate_tables)

add_custom_target(check-size
        COMMAND ${CMAKE_COMMAND} -E echo "=== Library Size Report ==="
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ls -lh $<TARGET_FILE:mywctype_static>
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND size $<TARGET_FILE:mywctype_static>
        DEPENDS mywctype_static
        COMMENT "Checking library size..."
)
