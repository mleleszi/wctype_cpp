#include "generator.h"

int main() {
  const auto entries = read_unicode_data("unicodedata/UnicodeData.txt");
  const auto properties = parse_unicode_data(entries);
  const auto table = build_lookup_tables(properties);
  generate_code(table);
}
