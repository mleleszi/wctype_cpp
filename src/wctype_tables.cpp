#include "wctype_table.h"

// Force the tables to be included in the static library
extern "C" {
const uint16_t *get_level1_table() { return level1; }
const uint8_t *get_level2_table() { return level2; }
uint8_t lookup_properties_wrapper(wint_t wc) { return lookup_properties(wc); }
}