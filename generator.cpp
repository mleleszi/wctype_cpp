#include "generator.h"

int main() {
    const auto entries = read_unicode_data("unicodedata/UnicodeData.txt");
    const auto properties = parse_unicode_data(entries);
    const auto table = build_two_level_table(properties);
    generate_code(table);
}
