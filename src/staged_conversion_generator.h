#ifndef CASE_MAPPING_STAGED_GENERATOR_H
#define CASE_MAPPING_STAGED_GENERATOR_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct StagedCaseMappingEntry {
  uint32_t codepoint;
  uint32_t uppercase; // 0 if no uppercase mapping exists
  uint32_t lowercase; // 0 if no lowercase mapping exists

  StagedCaseMappingEntry(uint32_t cp, uint32_t upper, uint32_t lower)
    : codepoint(cp), uppercase(upper), lowercase(lower) {
  }
};

inline std::optional<StagedCaseMappingEntry>
parse_staged_case_mapping_line(const std::string &line) {
  if (line.empty() || line[0] == '#') {
    return std::nullopt;
  }

  std::istringstream iss(line);
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

  uint32_t codepoint = std::stoul(fields[0], nullptr, 16);

  // Skip surrogate pairs
  if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
    return std::nullopt;
  }

  uint32_t uppercase = 0;
  uint32_t lowercase = 0;

  if (!fields[12].empty()) {
    uppercase = std::stoul(fields[12], nullptr, 16);
  }

  if (!fields[13].empty()) {
    lowercase = std::stoul(fields[13], nullptr, 16);
  }

  return StagedCaseMappingEntry(codepoint, uppercase, lowercase);
}

inline std::vector<StagedCaseMappingEntry>
read_staged_case_mappings(const std::string_view filename) {
  std::vector<StagedCaseMappingEntry> entries;

  std::ifstream file(filename.data());
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + std::string(filename));
  }

  std::string line;
  while (std::getline(file, line)) {
    if (auto entry = parse_staged_case_mapping_line(line)) {
      entries.push_back(*entry);
    }
  }

  return entries;
}

struct StagedCaseTable {
  std::vector<uint16_t> level1; // Maps codepoint >> 8 to level2 offset
  std::vector<uint32_t> level2; // Target codepoints (or 0 for identity)
};

inline StagedCaseTable build_staged_table(
    const std::vector<StagedCaseMappingEntry> &entries,
    bool to_upper) {
  constexpr uint32_t unicode_max = 0x110000;
  constexpr uint32_t block_size = 256;
  constexpr uint32_t num_blocks = unicode_max / block_size;

  using Block = std::array<uint32_t, block_size>;

  std::unordered_map<uint32_t, uint32_t> mappings;
  for (const auto &entry : entries) {
    if (to_upper) {
      if (entry.uppercase != 0) {
        mappings[entry.codepoint] = entry.uppercase;
      }
    } else {
      if (entry.lowercase != 0) {
        mappings[entry.codepoint] = entry.lowercase;
      }
    }
  }

  std::cout << (to_upper ? "To-upper" : "To-lower") << " mappings: " << mappings
      .size() << "\n";

  // Map: block content -> block index in level2
  std::map<Block, size_t> blocks;
  std::vector<uint16_t> level1;
  std::vector<uint32_t> level2;

  for (uint32_t block_num = 0; block_num < num_blocks; ++block_num) {
    Block block_content;
    for (uint32_t offset = 0; offset < block_size; ++offset) {
      uint32_t codepoint = (block_num << 8) | offset;
      auto it = mappings.find(codepoint);
      if (it != mappings.end()) {
        block_content[offset] = it->second; // Store target codepoint
      } else {
        block_content[offset] = 0; // No mapping - will return identity
      }
    }

    if (auto it = blocks.find(block_content); it != blocks.end()) {
      level1.push_back(it->second);
    } else {
      const size_t block_index = level2.size();
      blocks[block_content] = block_index;

      level2.insert(level2.end(), block_content.begin(), block_content.end());
      level1.push_back(block_index);
    }
  }

  uint32_t max_block = 0;
  for (const auto &[codepoint, _] : mappings) {
    max_block = std::max(max_block, codepoint >> 8);
  }
  max_block = std::min(max_block + 1, num_blocks - 1);
  level1.resize(max_block + 1);

  std::cout << "  Level 1 entries: " << level1.size() << "\n";
  std::cout << "  Level 2 entries: " << level2.size() << "\n";
  std::cout << "  Unique blocks: " << blocks.size() << "\n";
  std::cout << "  Block sharing: " << (0x1100 - blocks.size()) <<
      " blocks saved\n";
  std::cout << "  Memory: Level1=" << (level1.size() * 2) << " bytes, "
      << "Level2=" << (level2.size() * 4) << " bytes\n";

  return {level1, level2};
}

inline void generate_level1_inc(const std::vector<uint16_t> &level1,
                                const std::string_view filename) {
  std::ofstream f(filename.data());
  f << "// Auto-generated level1 table\n";
  for (size_t i = 0; i < level1.size(); i += 8) {
    f << "  ";
    for (size_t j = i; j < i + 8 && j < level1.size(); ++j) {
      f << std::setw(5) << level1[j];
      if (j + 1 < level1.size()) {
        f << ",";
      }
    }
    f << "\n";
  }
}

inline void generate_level2_inc(const std::vector<uint32_t> &level2,
                                const std::string_view filename) {
  std::ofstream f(filename.data());
  f << "// Auto-generated level2 table\n";
  for (size_t i = 0; i < level2.size(); i += 8) {
    f << "  ";
    for (size_t j = i; j < i + 8 && j < level2.size(); ++j) {
      f << "0x" << std::hex << std::setw(8) << std::setfill('0') << level2[j] <<
          std::dec << std::setfill(' ');
      if (j + 1 < level2.size()) {
        f << ", ";
      }
    }
    f << "\n";
  }
}

inline void generate_staged_case_code(
    const StagedCaseTable &to_upper_table,
    const StagedCaseTable &to_lower_table) {
  generate_level1_inc(to_upper_table.level1,
                      "case_mapping_upper_level1_staged.inc");
  generate_level2_inc(to_upper_table.level2,
                      "case_mapping_upper_level2_staged.inc");
  generate_level1_inc(to_lower_table.level1,
                      "case_mapping_lower_level1_staged.inc");
  generate_level2_inc(to_lower_table.level2,
                      "case_mapping_lower_level2_staged.inc");

  std::ofstream f("case_mapping_staged.h");
  if (!f.is_open()) {
    throw std::runtime_error("Cannot create case_mapping_staged.h");
  }

  f << R"(// Auto-generated by case_mapping_staged_generator.h
// DO NOT EDIT MANUALLY

#ifndef CASE_MAPPING_STAGED_H
#define CASE_MAPPING_STAGED_H

#include <stddef.h>
#include <stdint.h>
#include <wctype.h>


// Level 1: maps upper 8 bits to level2 offset
inline constexpr uint16_t to_upper_level1[)" << to_upper_table.level1.size() <<
      R"(] = {
#include "case_mapping_upper_level1_staged.inc"
};

// Level 2: target codepoints (0 = no mapping, return identity)
inline constexpr uint32_t to_upper_level2[)" << to_upper_table.level2.size() <<
      R"(] = {
#include "case_mapping_upper_level2_staged.inc"
};

// Level 1: maps upper 8 bits to level2 offset
inline constexpr uint16_t to_lower_level1[)" << to_lower_table.level1.size() <<
      R"(] = {
#include "case_mapping_lower_level1_staged.inc"
};

// Level 2: target codepoints (0 = no mapping, return identity)
inline constexpr uint32_t to_lower_level2[)" << to_lower_table.level2.size() <<
      R"(] = {
#include "case_mapping_lower_level2_staged.inc"
};

inline wint_t towupper_impl(wint_t wc) {
  if (wc == WEOF) {
    return WEOF;
  }

  if (wc < 0 || wc > 0x10FFFF || (wc >= 0xD800 && wc <= 0xDFFF)) {
    return wc;
  }

  // ASCII fast path
  if (wc >= 'a' && wc <= 'z') {
    return wc - 32;
  }
  if (wc <= 0x7F) {
    return wc;  // ASCII non-letters
  }

  unsigned l1_idx = wc >> 8;
  if (l1_idx >= )" << to_upper_table.level1.size() << R"() {
    return wc;  // Beyond table range
  }

  unsigned l2_offset = to_upper_level1[l1_idx];
  unsigned l2_idx = l2_offset + (wc & 0xFF);

  uint32_t target = to_upper_level2[l2_idx];
  return target ? target : wc;
}

inline wint_t towlower_impl(wint_t wc) {
  if (wc == WEOF) {
    return WEOF;
  }

  // Out of valid Unicode range
  if (wc < 0 || wc > 0x10FFFF || (wc >= 0xD800 && wc <= 0xDFFF)) {
    return wc;
  }

  // ASCII fast path
  if (wc >= 'A' && wc <= 'Z') {
    return wc + 32;
  }
  if (wc <= 0x7F) {
    return wc;  // ASCII non-letters
  }

  unsigned l1_idx = wc >> 8;
  if (l1_idx >= )" << to_lower_table.level1.size() << R"() {
    return wc;  // Beyond table range
  }

  unsigned l2_offset = to_lower_level1[l1_idx];
  unsigned l2_idx = l2_offset + (wc & 0xFF);

  uint32_t target = to_lower_level2[l2_idx];
  return target ? target : wc;
}

#endif // CASE_MAPPING_STAGED_H
)";

  f.close();
}

#endif // CASE_MAPPING_STAGED_GENERATOR_H