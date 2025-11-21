#include "wctype_table.h"
#include "case_mapping_staged.h"

// Force the tables to be included in the static library
extern "C" {
// const uint16_t *get_level1_table() { return level1; }
// const uint8_t *get_level2_table() { return level2; }
const uint16_t *get_upper_level1_table() { return to_upper_level1; };
const uint32_t *get_upper_level2_table() { return to_upper_level2; };
const uint16_t *get_lower_level1_table() { return to_lower_level1; };
const uint32_t *get_lower_level2_table() { return to_lower_level2; };
}