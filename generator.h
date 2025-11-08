#ifndef GENERATOR_H
#define GENERATOR_H

#include <unordered_map>
#include <cstdint>
#include <string_view>
#include <string>
#include <algorithm>
#include <array>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>

enum PropertyBits : uint16_t {
    PROP_UPPER = 1 << 0,
    PROP_LOWER = 1 << 1,
    PROP_ALPHA = 1 << 2,
    PROP_DIGIT = 1 << 3,
    PROP_XDIGIT = 1 << 4,
    PROP_SPACE = 1 << 5,
    PROP_PRINT = 1 << 6,
    PROP_GRAPH = 1 << 7,
    PROP_BLANK = 1 << 8,
    PROP_CNTRL = 1 << 9,
    PROP_PUNCT = 1 << 10,
};

// Non-whitespace spaces in C.UTF-8
constexpr std::array<uint32_t, 3> NON_WHITESPACE_SPACES = {
    0x00A0, // NO-BREAK SPACE
    0x2007, // FIGURE SPACE
    0x202F, // NARROW NO-BREAK SPACE
};

inline bool is_non_whitespace_space(uint32_t codepoint) {
    return std::ranges::any_of(NON_WHITESPACE_SPACES.begin(), NON_WHITESPACE_SPACES.end(),
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

inline std::vector<UnicodeEntry> read_unicode_data(const std::string_view filename) {
    std::vector<UnicodeEntry> entries;

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + std::string(filename));
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        // Parse the line
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
inline void handle_ranges(std::unordered_map<uint32_t, uint16_t> &properties,
                          const std::vector<UnicodeEntry> &entries) {
    struct RangeInfo {
        uint32_t start;
        uint16_t props;
    };

    std::optional<RangeInfo> range_info;
    for (const auto &entry: entries) {
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
inline uint16_t get_props(const UnicodeEntry &entry) {
    const uint32_t codepoint = entry.codepoint;
    const std::string_view category = entry.category;

    uint16_t props = 0;
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

            // C.UTF-8 only considers ASCII as digits
            if (codepoint >= 0x0030 && codepoint <= 0x0039) {
                props |= PROP_DIGIT;
            }
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

    // Graph = print but not space
    if ((props & PROP_PRINT) && !(props & PROP_SPACE)) {
        props |= PROP_GRAPH;
    }

    // Special case: NO-BREAK SPACE is graphical in glibc C.UTF-8
    if (codepoint == 0x00A0) {
        props |= PROP_GRAPH;
    }

    return props;
}

// Handle special cases not parseable from UnicodeData.txt
void handle_special_cases(std::unordered_map<uint32_t, uint16_t> &properties) {
    // Hexadecimal digits (0-9, A-F, a-f)
    for (uint32_t c = 0x0030; c <= 0x0039; ++c) {
        // 0-9
        properties[c] |= PROP_XDIGIT;
    }
    for (uint32_t c = 0x0041; c <= 0x0046; ++c) {
        // A-F
        properties[c] |= PROP_XDIGIT;
    }
    for (uint32_t c = 0x0061; c <= 0x0066; ++c) {
        // a-f
        properties[c] |= PROP_XDIGIT;
    }

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
inline std::unordered_map<uint32_t, uint16_t> parse_unicode_data(const std::vector<UnicodeEntry> &entries) {
    std::unordered_map<uint32_t, uint16_t> properties;

    for (const auto &entry: entries) {
        const uint32_t codepoint = entry.codepoint;

        // Skip surrogate pairs
        if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
            continue;
        }

        properties[codepoint] = get_props(entry);
    }

    handle_ranges(properties, entries);
    handle_special_cases(properties);

    return properties;
}


#endif //GENERATOR_H
