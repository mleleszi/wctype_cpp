#include <iostream>

#include "generator.h"

uint16_t lookup_properties(const StagedLookupTable &lookup_table, const wint_t wc) {
    // Out of Unicode range
    if (wc > 0x10FFFF || (wc >= 0xD800 && wc <= 0xDFFF)) {
        return 0;
    }
    const unsigned l1_idx = wc >> 8;
    if (l1_idx >= 4352) {
        return 0;
    }

    const unsigned l2_offset = lookup_table.level1[l1_idx];
    const unsigned l2_idx = l2_offset + (wc & 0xFF);

    return lookup_table.level2[l2_idx];
}

int main() {
    auto entries = read_unicode_data("unicodedata/UnicodeData.txt");
    auto properties = parse_unicode_data(entries);
    auto table = build_two_level_table(properties);

    generate_cpp_tables(table);

    const wint_t cp = 0x0041;
    auto prop = lookup_properties(table, cp);
    std::cout << (prop & PROP_LOWER) << std::endl;
    return 0;
}
