#include "generator.h"
#include "staged_conversion_generator.h"
#include "conversion_generator.h"

int main() {
  std::string_view filename = "unicodedata/UnicodeData.txt";

  const auto entries = read_unicode_data(filename);
  const auto properties = parse_unicode_data(entries);
  const auto table = build_lookup_tables(properties);
  generate_code(table);

  const auto case_entries = read_case_mappings(filename);
  auto [to_upper, to_lower] = build_case_mapping_tables(case_entries);
  generate_case_mapping_code(to_upper, to_lower);

  const auto staged_case_mappings = read_staged_case_mappings(filename);
  auto to_upper_table = build_staged_table(staged_case_mappings, true);
  auto to_lower_table = build_staged_table(staged_case_mappings, false);
  generate_staged_case_code(to_upper_table, to_lower_table);
}


