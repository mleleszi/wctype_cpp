from enum import IntFlag
from dataclasses import dataclass
from collections import defaultdict


class PropertyFlag(IntFlag):
    PROP_UPPER = 1 << 0
    PROP_LOWER = 1 << 1
    PROP_ALPHA = 1 << 2
    PROP_SPACE = 1 << 3
    PROP_PRINT = 1 << 4
    PROP_BLANK = 1 << 5
    PROP_CNTRL = 1 << 6
    PROP_PUNCT = 1 << 7


NON_WHITESPACE_SPACE = [
    0x00A0,  # NO-BREAK SPACE
    0x2007,  # FIGURE SPACE
    0x202F,  # NARROW NO-BREAK SPACE
]


@dataclass
class UnicodeEntry:
    codepoint: int
    name: str
    category: str


def read_unicode_data(filename: str) -> list[UnicodeEntry]:
    """Read Unicode data from file and return list of entries."""
    entries: list[UnicodeEntry] = []

    try:
        with open(filename, "r", encoding="utf-8") as file:
            for line in file:
                line = line.strip()

                # Skip empty lines and comments
                if not line or line.startswith("#"):
                    continue

                # Split by semicolon
                fields = line.split(";")

                # Need at least 3 fields (codepoint, name, category)
                if len(fields) < 3:
                    continue

                codepoint_str = fields[0].strip()
                name = fields[1].strip()
                category = fields[2].strip()

                # Parse hex codepoint
                codepoint = int(codepoint_str, 16)

                entries.append(UnicodeEntry(codepoint, name, category))

    except FileNotFoundError:
        raise RuntimeError(f"Cannot open file: {filename}")

    return entries


from dataclasses import dataclass

# Non-whitespace spaces in C.UTF-8
NON_WHITESPACE_SPACES = {0x00A0, 0x2007, 0x202F}


def is_non_whitespace_space(codepoint: int) -> bool:
    return codepoint in NON_WHITESPACE_SPACES


def handle_ranges(
    properties: defaultdict[int, int], entries: list[UnicodeEntry]
) -> None:
    """Handle Unicode ranges defined by <First> and <Last>."""
    range_start: int | None = None
    range_props: int | None = None

    for entry in entries:
        if ", First>" in entry.name:
            range_start = entry.codepoint
            range_props = properties[entry.codepoint]
        elif ", Last>" in entry.name and range_start and range_props:
            for cp in range(range_start, entry.codepoint + 1):
                properties[cp] = range_props
            range_start = None
            range_props = None


def get_props(entry: UnicodeEntry) -> int:
    """Create the properties flag for a given UnicodeEntry."""
    codepoint = entry.codepoint
    category = entry.category

    props = 0

    # Letter
    if category.startswith("L"):
        props |= PropertyFlag.PROP_ALPHA
        if category == "Lu":
            props |= PropertyFlag.PROP_UPPER
        elif category == "Ll":
            props |= PropertyFlag.PROP_LOWER

    # Number
    elif category.startswith("N"):
        if category in ("Nd", "Nl"):
            props |= PropertyFlag.PROP_ALPHA

    # Punctuation
    elif category.startswith("P"):
        props |= PropertyFlag.PROP_PUNCT

    # Symbol
    elif category.startswith("S"):
        props |= PropertyFlag.PROP_PUNCT  # Considered punctuation in C.UTF8

    # Separator
    elif category.startswith("Z"):
        if not is_non_whitespace_space(codepoint):
            props |= PropertyFlag.PROP_SPACE
            if category == "Zs":
                props |= PropertyFlag.PROP_BLANK

    # Control
    elif category.startswith("C"):
        if category == "Cc":
            props |= PropertyFlag.PROP_CNTRL

    # Print = all except control, unassigned, surrogate, format
    if category not in ("Cc", "Cs", "Cn", "Cf"):
        props |= PropertyFlag.PROP_PRINT

    return props


def handle_special_cases(properties: defaultdict[int, int]) -> None:
    """Handle special cases not parseable from UnicodeData.txt."""
    # ASCII whitespace characters
    properties[0x0020] |= PropertyFlag.PROP_SPACE  # SPACE
    properties[0x0009] |= PropertyFlag.PROP_SPACE  # TAB
    properties[0x000A] |= PropertyFlag.PROP_SPACE  # LINE FEED
    properties[0x000D] |= PropertyFlag.PROP_SPACE  # CARRIAGE RETURN
    properties[0x000B] |= PropertyFlag.PROP_SPACE  # VERTICAL TAB
    properties[0x000C] |= PropertyFlag.PROP_SPACE  # FORM FEED

    # Blank (horizontal spacing only)
    properties[0x0020] |= PropertyFlag.PROP_BLANK  # SPACE
    properties[0x0009] |= PropertyFlag.PROP_BLANK  # TAB


def assert_disjoint_invariants(
    properties: defaultdict[int, int], entries: list[UnicodeEntry]
) -> None:
    """Assert disjoint set invariants defined by C standard."""
    category_map = {entry.codepoint: entry.category for entry in entries}

    for codepoint, props in properties.items():
        category = category_map.get(codepoint, "")
        is_letter = category.startswith("L")
        is_digit = category == "Nd"

        # 1. Digit & alpha must be disjoint
        assert not (is_letter and is_digit), f"U+{codepoint:04X}: letter+digit"

        # 2. Punct & alpha must be disjoint
        assert not (
            (props & PropertyFlag.PROP_PUNCT) and (props & PropertyFlag.PROP_ALPHA)
        ), f"U+{codepoint:04X}: punct+alpha"

        # 3. Punct & digit must be disjoint
        assert not ((props & PropertyFlag.PROP_PUNCT) and is_digit), (
            f"U+{codepoint:04X}: punct+digit"
        )

        # 4. Cntrl & print must be disjoint
        assert not (
            (props & PropertyFlag.PROP_CNTRL) and (props & PropertyFlag.PROP_PRINT)
        ), f"U+{codepoint:04X}: cntrl+print"


def parse_unicode_data(entries: list[UnicodeEntry]) -> defaultdict[int, int]:
    """Return codepoint -> property flag mappings."""
    properties: defaultdict[int, int] = defaultdict(int)

    for entry in entries:
        codepoint = entry.codepoint

        # Skip surrogate pairs
        if 0xD800 <= codepoint <= 0xDFFF:
            continue

        properties[codepoint] = get_props(entry)

    handle_ranges(properties, entries)
    handle_special_cases(properties)
    assert_disjoint_invariants(properties, entries)

    return properties


@dataclass
class StagedLookupTable:
    level1: list[int]  # Maps codepoint >> 8 to level2 offset
    level2: list[int]  # Actual properties


def build_lookup_tables(properties: defaultdict[int, int]) -> StagedLookupTable:
    """Build two-level lookup tables with block sharing."""
    UNICODE_MAX = 0x110000
    BLOCK_SIZE = 256
    NUM_BLOCKS = UNICODE_MAX // BLOCK_SIZE

    # Map: block content (as tuple) -> block index in level2
    blocks: defaultdict[tuple[int, ...], int] = defaultdict(int)
    level1: list[int] = []
    level2: list[int] = []

    for block_num in range(NUM_BLOCKS):
        # Extract properties for this 256-character block
        block_content = tuple(
            properties.get((block_num << 8) | offset, 0) for offset in range(BLOCK_SIZE)
        )

        # Check if we've seen this block before (block sharing)
        if block_content in blocks:
            # Reuse existing block
            level1.append(blocks[block_content])
        else:
            # New block - add to level2
            block_index = len(level2)
            blocks[block_content] = block_index

            # Add all 256 entries to level2
            level2.extend(block_content)
            level1.append(block_index)

    # Only keep level1 entries we actually need (up to the highest used codepoint)
    max_block = max((cp >> 8 for cp in properties.keys()), default=0)
    level1 = level1[: max_block + 1]

    print("Table statistics:")
    print(f"  Level 1 entries: {len(level1)}")
    print(f"  Level 2 entries: {len(level2)}")
    print(f"  Unique blocks: {len(blocks)}")
    print(f"  Block sharing: {0x1100 - len(blocks)} blocks saved")
    print(f"  Memory: Level1={len(level1) * 2} bytes, Level2={len(level2) * 1} bytes")

    return StagedLookupTable(level1, level2)


def generate_code(lookup_table: StagedLookupTable) -> None:
    """Generate C header files with lookup tables."""
    level1 = lookup_table.level1
    level2 = lookup_table.level2

    # Generate property bit definitions
    with open("wctype_properties.h", "w") as f:
        f.write("""// Auto-generated by generator.h
// DO NOT EDIT MANUALLY

#ifndef WCTYPE_PROPERTIES_H
#define WCTYPE_PROPERTIES_H

#include <stdint.h>

enum PropertyFlag : uint8_t {
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
""")

    # Generate level1 table
    with open("wctype_level1.inc", "w") as f:
        f.write("// Auto-generated level1 table\n")
        for i in range(0, len(level1), 8):
            f.write("  ")
            for j in range(i, min(i + 8, len(level1))):
                f.write(f"{level1[j]:5d}")
                if j + 1 < len(level1):
                    f.write(",")
            f.write("\n")

    # Generate level2 table
    with open("wctype_level2.inc", "w") as f:
        f.write("// Auto-generated level2 table\n")
        for i in range(0, len(level2), 8):
            f.write("  ")
            for j in range(i, min(i + 8, len(level2))):
                f.write(f"0x{level2[j]:02x}")
                if j + 1 < len(level2):
                    f.write(", ")
            f.write("\n")

    # Generate the main header with lookup function
    with open("wctype_table.h", "w") as f:
        f.write(f"""// Auto-generated by generator.h
// DO NOT EDIT MANUALLY

#ifndef WCTYPE_TABLE_H
#define WCTYPE_TABLE_H

#include "wctype_properties.h"
#include <stdint.h>
#include <wctype.h>

// Level 1 table: maps upper 8 bits to level2 offset
inline constexpr uint16_t level1[{len(level1)}] = {{
#include "wctype_level1.inc"
}};

// Level 2 table: property bitfields for each character
inline constexpr uint8_t level2[{len(level2)}] = {{
#include "wctype_level2.inc"
}};

inline uint8_t lookup_properties(wint_t wc) {{
  // Out of Unicode range
  if (wc > 0x10FFFF || (wc >= 0xD800 && wc <= 0xDFFF)) {{
    return 0;
  }}

  unsigned l1_idx = wc >> 8;
  if (l1_idx >= {len(level1)}) {{
    return 0;
  }}

  unsigned l2_offset = level1[l1_idx];
  unsigned l2_idx = l2_offset + (wc & 0xFF);

  return level2[l2_idx];
}}

#endif // WCTYPE_TABLE_H
""")


if __name__ == "__main__":
    entries = read_unicode_data("../../unicodedata/UnicodeData.txt")
    properties = parse_unicode_data(entries)
    assert_disjoint_invariants(properties, entries)
    tables = build_lookup_tables(properties)
    generate_code(tables)
