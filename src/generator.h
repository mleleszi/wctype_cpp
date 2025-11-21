#ifndef GENERATOR_H
#define GENERATOR_H

#include <unordered_map>
#include <string_view>
#include <string>
#include <algorithm>
#include <array>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>
#include <map>
#include <ranges>
#include <iomanip>
#include <iostream>

enum PropertyBits : uint8_t {
  PROP_UPPER = 1 << 0,
  PROP_LOWER = 1 << 1,
  PROP_ALPHA = 1 << 2,
  PROP_SPACE = 1 << 3,
  PROP_PRINT = 1 << 4,
  PROP_BLANK = 1 << 5,
  PROP_CNTRL = 1 << 6,
  PROP_PUNCT = 1 << 7,
};

// Non-whitespace spaces in C.UTF-8
constexpr std::array<uint32_t, 3> NON_WHITESPACE_SPACES = {
    0x00A0, // NO-BREAK SPACE
    0x2007, // FIGURE SPACE
    0x202F, // NARROW NO-BREAK SPACE
};

inline bool is_non_whitespace_space(uint32_t codepoint) {
  return std::ranges::any_of(NON_WHITESPACE_SPACES,
                             [codepoint](auto non_whitespace) {
                               return non_whitespace == codepoint;
                             });
}

inline bool starts_with(const std::string_view str, const char prefix) {
  assert(!str.empty());
  return str[0] == prefix;
}

struct UnicodeEntry {
  uint32_t codepoint;
  std::string name;
  std::string category;

  UnicodeEntry(const uint32_t cp, std::string n, std::string cat)
    : codepoint(cp), name(std::move(n)), category(std::move(cat)) {
  }
};

inline std::vector<UnicodeEntry> read_unicode_data(
    const std::string_view filename) {
  std::vector<UnicodeEntry> entries;

  std::ifstream file(filename.data());
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + std::string(filename));
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string codepoint_str, name, category;

    if (!std::getline(iss, codepoint_str, ';') ||
        !std::getline(iss, name, ';') ||
        !std::getline(iss, category, ';')) {
      continue;
    }

    uint32_t codepoint = std::stoul(codepoint_str, nullptr, 16);
    entries.emplace_back(codepoint, name, category);
  }
  return entries;
}

// Handles Unicode ranges defined by <First> and <Last>
inline void handle_ranges(std::unordered_map<uint32_t, uint8_t> &properties,
                          const std::vector<UnicodeEntry> &entries) {
  struct RangeInfo {
    uint32_t start;
    uint8_t props;
  };

  std::optional<RangeInfo> range_info;
  for (const auto &entry : entries) {
    if (entry.name.find(", First>") != std::string::npos) {
      range_info = {entry.codepoint, properties[entry.codepoint]};
    } else if (entry.name.find(", Last>") != std::string::npos && range_info) {
      // Fill in the range
      for (uint32_t cp = range_info->start; cp <= entry.codepoint; ++cp) {
        properties[cp] = range_info->props;
      }
      range_info.reset();
    }
  }
}

// Creates the properties flag for a given UnicodeEntry
inline uint8_t get_props(const UnicodeEntry &entry) {
  const uint32_t codepoint = entry.codepoint;
  const std::string_view category = entry.category;

  uint8_t props = 0;
  // Letter
  if (starts_with(category, 'L')) {
    props |= PROP_ALPHA;

    // Uppercase
    if (category == "Lu") {
      props |= PROP_UPPER;
    }
    // Lowercase
    else if (category == "Ll") {
      props |= PROP_LOWER;
    }
  }
  // Number
  else if (starts_with(category, 'N')) {
    if (category == "Nd") {
      props |= PROP_ALPHA;
    } else if (category == "Nl") {
      props |= PROP_ALPHA;
    }
  }
  // Punctuation
  else if (starts_with(category, 'P')) {
    props |= PROP_PUNCT;
  }
  // Symbol
  else if (starts_with(category, 'S')) {
    props |= PROP_PUNCT; // Considered punctuation in C.UTF8
  }
  // Separator
  else if (starts_with(category, 'Z')) {
    if (!is_non_whitespace_space(codepoint)) {
      props |= PROP_SPACE;
      // Space separator
      if (category == "Zs") {
        props |= PROP_BLANK;
      }
    }
  }
  // Control
  else if (starts_with(category, 'C')) {
    if (category == "Cc") {
      props |= PROP_CNTRL;
    }
  }

  // Print = all except control, unassigned, surrogate, format
  if (category != "Cc" && category != "Cs" &&
      category != "Cn" && category != "Cf") {
    props |= PROP_PRINT;
  }

  return props;
}

// Handle special cases not parseable from UnicodeData.txt
inline void handle_special_cases(
    std::unordered_map<uint32_t, uint8_t> &properties) {
  // ASCII whitespace characters
  properties[0x0020] |= PROP_SPACE; // SPACE
  properties[0x0009] |= PROP_SPACE; // TAB
  properties[0x000A] |= PROP_SPACE; // LINE FEED
  properties[0x000D] |= PROP_SPACE; // CARRIAGE RETURN
  properties[0x000B] |= PROP_SPACE; // VERTICAL TAB
  properties[0x000C] |= PROP_SPACE; // FORM FEED

  // Blank (horizontal spacing only)
  properties[0x0020] |= PROP_BLANK; // SPACE
  properties[0x0009] |= PROP_BLANK; // TAB
}


// Returns codepoint -> property flag mappings
inline std::unordered_map<uint32_t, uint8_t> parse_unicode_data(
    const std::vector<UnicodeEntry> &entries) {
  std::unordered_map<uint32_t, uint8_t> properties;

  for (const auto &entry : entries) {
    const uint32_t codepoint = entry.codepoint;

    // Skip surrogate pairs
    if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
      continue;
    }

    properties[codepoint] = get_props(entry);
  }

  handle_ranges(properties, entries);
  handle_special_cases(properties);

  // TODO: assert disjoint set invariants defined by C standard
  // digit & alpha must be disjoint
  // punct & alpha must be disjoint
  // punct & digit must be disjoint
  // cntrl & print must be disjoint

  return properties;
}

struct StagedLookupTable {
  std::vector<uint16_t> level1; // Maps codepoint >> 8 to level2 offset
  std::vector<uint8_t> level2; // Actual properties
};

inline StagedLookupTable build_lookup_tables(
    const std::unordered_map<uint32_t, uint8_t> &properties) {
  constexpr uint32_t unicode_max = 0x110000;
  constexpr uint32_t block_size = 256;
  constexpr uint32_t num_blocks = unicode_max / block_size;

  // Block type: 256 property values
  using Block = std::array<uint8_t, block_size>;

  // Map: block content -> block index in level2
  std::map<Block, size_t> blocks;
  std::vector<uint16_t> level1;
  std::vector<uint8_t> level2;

  for (uint32_t block_num = 0; block_num < num_blocks; ++block_num) {
    // Extract properties for this 256-character block
    Block block_content;
    for (uint32_t offset = 0; offset < block_size; ++offset) {
      uint32_t codepoint = block_num << 8 | offset;
      auto it = properties.find(codepoint);
      const uint8_t props = it != properties.end() ? it->second : 0;
      block_content[offset] = props;
    }

    // Check if we've seen this block before (block sharing)
    if (auto it = blocks.find(block_content); it != blocks.end()) {
      // Reuse existing block
      level1.push_back(it->second);
    } else {
      // New block - add to level2
      const size_t block_index = level2.size();
      blocks[block_content] = block_index;

      // Add all 256 entries to level2
      level2.insert(level2.end(), block_content.begin(), block_content.end());
      level1.push_back(block_index);
    }
  }
  // Only keep level1 entries we actually need (up to the highest used codepoint)
  uint32_t max_block = 0;
  for (const auto &codepoint : properties | std::views::keys) {
    max_block = std::max(max_block, codepoint >> 8);
  }
  level1.resize(max_block + 1);

  std::cout << "Table statistics:\n";
  std::cout << "  Level 1 entries: " << level1.size() << "\n";
  std::cout << "  Level 2 entries: " << level2.size() << "\n";
  std::cout << "  Unique blocks: " << blocks.size() << "\n";
  std::cout << "  Block sharing: " << (0x1100 - blocks.size()) <<
      " blocks saved\n";
  std::cout << "  Memory: Level1=" << (level1.size() * 2) << " bytes, "
      << "Level2=" << (level2.size() * 1) << " bytes\n";

  return {level1, level2};
}

inline void generate_code(const StagedLookupTable &lookup_table) {
  auto level1 = lookup_table.level1;
  auto level2 = lookup_table.level2;
  // Generate property bit definitions
  {
    std::ofstream f("wctype_properties.h");
    f << R"(// Auto-generated by generator.h
// DO NOT EDIT MANUALLY

#ifndef WCTYPE_PROPERTIES_H
#define WCTYPE_PROPERTIES_H

#include <stdint.h>

enum PropertyBits : uint8_t {
  PROP_UPPER = 1 << 0,
  PROP_LOWER = 1 << 1,
  PROP_ALPHA = 1 << 2,
  PROP_SPACE = 1 << 3,
  PROP_PRINT = 1 << 4,
  PROP_BLANK = 1 << 5,
  PROP_CNTRL = 1 << 6,
  PROP_PUNCT = 1 << 7,
};

#endif // WCTYPE_PROPERTIES_H
)";
  }
  // Generate level1 table
  {
    std::ofstream f("wctype_level1.inc");
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
  // Generate level2 table
  {
    std::ofstream f("wctype_level2.inc");
    f << "// Auto-generated level2 table\n";
    for (size_t i = 0; i < level2.size(); i += 8) {
      f << "  ";
      for (size_t j = i; j < i + 8 && j < level2.size(); ++j) {
        f << "0x" << std::hex << std::setw(2) << std::setfill('0') <<
            static_cast<int>(level2[j])
            << std::dec <<
            std::setfill(' ');
        if (j + 1 < level2.size()) {
          f << ", ";
        }
      }
      f << "\n";
    }
  }
  // Generate the main header with lookup function
  {
    std::ofstream f("wctype_table.h");
    f << R"(// Auto-generated by generator.h
// DO NOT EDIT MANUALLY

#ifndef WCTYPE_TABLE_H
#define WCTYPE_TABLE_H

#include "wctype_properties.h"
#include <stdint.h>
#include <wctype.h>

// Level 1 table: maps upper 8 bits to level2 offset
inline constexpr uint16_t level1[)" << level1.size() << R"(] = {
#include "wctype_level1.inc"
};

// Level 2 table: property bitfields for each character
inline constexpr uint8_t level2[)" << level2.size() << R"(] = {
#include "wctype_level2.inc"
};

inline uint8_t lookup_properties(wint_t wc) {
  // Out of Unicode range
  if (wc > 0x10FFFF || (wc >= 0xD800 && wc <= 0xDFFF)) {
    return 0;
  }

  unsigned l1_idx = wc >> 8;
  if (l1_idx >= )" << level1.size() << R"() {
    return 0;
  }

  unsigned l2_offset = level1[l1_idx];
  unsigned l2_idx = l2_offset + (wc & 0xFF);

  return level2[l2_idx];
}

#endif // WCTYPE_TABLE_H
)";
  }
}


#endif //GENERATOR_H
