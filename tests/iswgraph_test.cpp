#include "wctype_test_base.h"
#include "mywctype.h"
#include <cwchar>

class GraphTest : public WctypeTest, public ::testing::WithParamInterface<TestCase> {
};

TEST_P(GraphTest, IsGraph) {
  auto test = GetParam();
  int my_result = my_wctype::iswgraph(test.codepoint);
  int std_result = std::iswgraph(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name << ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.name << ")";
}

INSTANTIATE_TEST_SUITE_P(GraphTests, GraphTest, ::testing::Values(
                           // Graphical characters (visible)
                           TestCase{0x0041, "LATIN CAPITAL LETTER A", true},
                           TestCase{0x0061, "LATIN SMALL LETTER A", true},
                           TestCase{0x0030, "DIGIT ZERO", true},
                           TestCase{0x0021, "EXCLAMATION MARK", true},
                           TestCase{0x007E, "TILDE", true},

                           // Non-graphical (whitespace and control)
                           TestCase{0x0020, "SPACE", false},
                           TestCase{0x0009, "CHARACTER TABULATION", false},
                           TestCase{0x000A, "LINE FEED", false},
                           TestCase{0x000D, "CARRIAGE RETURN", false},
                           TestCase{0x0000, "NULL", false},
                           TestCase{0x001F, "INFORMATION SEPARATOR ONE", false},

                           // Unicode graphical
                           TestCase{0x00E9, "LATIN SMALL LETTER E WITH ACUTE", true},
                           TestCase{0x0391, "GREEK CAPITAL LETTER ALPHA", true},
                           TestCase{0x4E00, "CJK UNIFIED IDEOGRAPH-4E00", true},
                           TestCase{0x2764, "HEAVY BLACK HEART", true},
                           TestCase{0x1F600, "GRINNING FACE", true},

                           // Unicode non-graphical
                           TestCase{0x00A0, "NO-BREAK SPACE", true},
                           TestCase{0x3000, "IDEOGRAPHIC SPACE", false}
                         ));
