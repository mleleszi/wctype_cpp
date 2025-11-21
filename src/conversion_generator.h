#ifndef CASE_MAPPING_GENERATOR_H
#define CASE_MAPPING_GENERATOR_H

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

struct CaseMapping {
  uint32_t from;
  uint32_t to;

  CaseMapping(const uint32_t f, const uint32_t t) : from(f), to(t) {
  }

  bool operator<(const CaseMapping &other) const { return from < other.from; }
};

struct CaseMappingEntry {
  uint32_t codepoint;
  uint32_t uppercase; // 0 if no uppercase mapping exists
  uint32_t lowercase; // 0 if no lowercase mapping exists

  CaseMappingEntry(const uint32_t cp, const uint32_t upper,
                   const uint32_t lower)
    : codepoint(cp), uppercase(upper), lowercase(lower) {
  }
};

inline std::optional<CaseMappingEntry>
parse_case_mapping_line(const std::string_view line) {
  if (line.empty() || line[0] == '#') {
    return std::nullopt;
  }

  std::istringstream iss(line.data());
  std::string field;
  std::vector<std::string> fields;

  while (std::getline(iss, field, ';')) {
    fields.push_back(field);
  }

  // UnicodeData.txt has 15 fields (0-14)
  // Field 0: Codepoint
  // Field 12: Uppercase mapping
  // Field 13: Lowercase mapping
  if (fields.size() < 14) {
    return std::nullopt;
  }

  const uint32_t codepoint = std::stoul(fields[0], nullptr, 16);

  // Skip surrogate pairs
  if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
    return std::nullopt;
  }

  uint32_t uppercase = 0;
  uint32_t lowercase = 0;

  // Parse uppercase mapping (field 12)
  if (!fields[12].empty()) {
    uppercase = std::stoul(fields[12], nullptr, 16);
  }

  // Parse lowercase mapping (field 13)
  if (!fields[13].empty()) {
    lowercase = std::stoul(fields[13], nullptr, 16);
  }

  return CaseMappingEntry(codepoint, uppercase, lowercase);
}

inline std::vector<CaseMappingEntry>
read_case_mappings(const std::string_view filename) {
  std::vector<CaseMappingEntry> entries;

  std::ifstream file(filename.data());
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + std::string(filename));
  }

  std::string line;
  while (std::getline(file, line)) {
    if (auto entry = parse_case_mapping_line(line)) {
      entries.push_back(*entry);
    }
  }

  return entries;
}

inline std::pair<std::vector<CaseMapping>, std::vector<CaseMapping>>
build_case_mapping_tables(const std::vector<CaseMappingEntry> &entries) {
  std::vector<CaseMapping> to_upper;
  std::vector<CaseMapping> to_lower;

  for (const auto &entry : entries) {
    // Add to-upper mapping if it exists and is non-identity
    if (entry.uppercase != 0 && entry.uppercase != entry.codepoint) {
      to_upper.emplace_back(entry.codepoint, entry.uppercase);
    }

    // Add to-lower mapping if it exists and is non-identity
    if (entry.lowercase != 0 && entry.lowercase != entry.codepoint) {
      to_lower.emplace_back(entry.codepoint, entry.lowercase);
    }
  }

  std::sort(to_upper.begin(), to_upper.end());
  std::sort(to_lower.begin(), to_lower.end());

  to_upper.erase(std::ranges::unique(to_upper,
                                     [](const CaseMapping &a,
                                        const CaseMapping &b) {
                                       return a.from == b.from;
                                     }).begin(),
                 to_upper.end());

  to_lower.erase(std::ranges::unique(to_lower,
                                     [](const CaseMapping &a,
                                        const CaseMapping &b) {
                                       return a.from == b.from;
                                     }).begin(),
                 to_lower.end());

  std::cout << "\nCase mapping statistics:\n";
  std::cout << "  To-upper mappings: " << to_upper.size() << "\n";
  std::cout << "  To-lower mappings: " << to_lower.size() << "\n";
  std::cout << "  Total memory: "
      << ((to_upper.size() + to_lower.size()) * sizeof(CaseMapping))
      << " bytes\n";

  return {to_upper, to_lower};
}

inline void generate_mapping_inc(const std::vector<CaseMapping> &mappings,
                                 const std::string_view filename) {
  std::ofstream f(filename.data());
  if (!f.is_open()) {
    throw std::runtime_error("Cannot create " + std::string(filename));
  }

  f << "// Auto-generated mapping table: " << mappings.size() << " entries\n";
  f << "// Format: {from_codepoint, to_codepoint}\n";

  for (size_t i = 0; i < mappings.size(); ++i) {
    f << "  {0x" << std::hex << std::setw(6) << std::setfill('0')
        << mappings[i].from << ", 0x" << std::setw(6) << std::setfill('0')
        << mappings[i].to << "}" << std::dec;

    if (i + 1 < mappings.size()) {
      f << ",";
    }
    f << "\n";
  }

  f.close();
}

inline void generate_case_mapping_code(const std::vector<CaseMapping> &to_upper,
                                       const std::vector<CaseMapping> &
                                       to_lower) {
  generate_mapping_inc(to_upper, "case_mapping_upper.inc");
  generate_mapping_inc(to_lower, "case_mapping_lower.inc");

  std::ofstream f("case_mapping.h");
  if (!f.is_open()) {
    throw std::runtime_error("Cannot create case_mapping.h");
  }

  f << R"(// Auto-generated by case_mapping_generator.h
// DO NOT EDIT MANUALLY

#ifndef CASE_MAPPING_H
#define CASE_MAPPING_H

#include <stdint.h>
#include <wctype.h>

struct CaseMapping {
  uint32_t from;
  uint32_t to;
};

inline constexpr CaseMapping to_upper_table[)" << to_upper.size() << R"(] = {
#include "wctype_case_mapping_upper.inc"
};

inline constexpr CaseMapping to_lower_table[)" << to_lower.size() << R"(] = {
#include "wctype_case_mapping_lower.inc"
};

inline const CaseMapping *find_mapping(const CaseMapping *table, size_t size,
                                       uint32_t codepoint) {
  size_t left = 0;
  size_t right = size;

  while (left < right) {
    size_t mid = left + (right - left) / 2;
    if (table[mid].from == codepoint) {
      return &table[mid];
    } else if (table[mid].from < codepoint) {
      left = mid + 1;
    } else {
      right = mid;
    }
  }
  return nullptr;
}

inline wint_t towupper_impl(wint_t wc) {
  if (wc == WEOF) {
    return WEOF;
  }

  // Out of valid Unicode range
  if (wc < 0 || wc > 0x10FFFF || (wc >= 0xD800 && wc <= 0xDFFF)) {
    return wc;
  }

  const CaseMapping *mapping =
      find_mapping(to_upper_table, )" << to_upper.size() << R"(, wc);

  return mapping ? mapping->to : wc;
}

inline wint_t towlower_impl(wint_t wc) {
  if (wc == WEOF) {
    return WEOF;
  }

  // Out of valid Unicode range
  if (wc < 0 || wc > 0x10FFFF || (wc >= 0xD800 && wc <= 0xDFFF)) {
    return wc;
  }

  const CaseMapping *mapping =
      find_mapping(to_lower_table, )" << to_lower.size() << R"(, wc);

  return mapping ? mapping->to : wc;
}

#endif // CASE_MAPPING_H
)";

  f.close();
}

#endif // CASE_MAPPING_GENERATOR_H