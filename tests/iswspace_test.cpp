#include "wctype_test_base.h"
#include "mywctype.h"
#include <cwchar>

class SpaceTest : public WctypeTest,
                  public ::testing::WithParamInterface<TestCase> {
};

TEST_P(SpaceTest, IsSpace) {
  auto test = GetParam();
  int my_result = my_wctype::iswspace(test.codepoint);
  int std_result = std::iswspace(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name <<
 ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.
name << ")";
}

INSTANTIATE_TEST_SUITE_P(SpaceTests, SpaceTest, ::testing::Values(
                           // ASCII whitespace
                           TestCase{0x0020, "SPACE", true},
                           TestCase{0x0009, "CHARACTER TABULATION (TAB)", true},
                           TestCase{0x000A, "LINE FEED", true},
                           TestCase{0x000B, "LINE TABULATION", true},
                           TestCase{0x000C, "FORM FEED", true},
                           TestCase{0x000D, "CARRIAGE RETURN", true},

                           // ASCII non-whitespace
                           TestCase{0x0041, "LATIN CAPITAL LETTER A", false},
                           TestCase{0x0030, "DIGIT ZERO", false},
                           TestCase{0x0021, "EXCLAMATION MARK", false},

                           // Unicode whitespace
                           TestCase{0x1680, "OGHAM SPACE MARK", true},
                           TestCase{0x2000, "EN QUAD", true},
                           TestCase{0x2001, "EM QUAD", true},
                           TestCase{0x2002, "EN SPACE", true},
                           TestCase{0x2003, "EM SPACE", true},
                           TestCase{0x2004, "THREE-PER-EM SPACE", true},
                           TestCase{0x2005, "FOUR-PER-EM SPACE", true},
                           TestCase{0x2006, "SIX-PER-EM SPACE", true},
                           TestCase{0x2008, "PUNCTUATION SPACE", true},
                           TestCase{0x2009, "THIN SPACE", true},
                           TestCase{0x200A, "HAIR SPACE", true},
                           TestCase{0x2028, "LINE SEPARATOR", true},
                           TestCase{0x2029, "PARAGRAPH SEPARATOR", true},
                           TestCase{0x205F, "MEDIUM MATHEMATICAL SPACE", true},
                           TestCase{0x3000, "IDEOGRAPHIC SPACE", true},

                           TestCase{0x202F, "NARROW NO-BREAK SPACE", false},
                           TestCase{0x0085, "NEXT LINE", false},
                           TestCase{0x00A0, "NO-BREAK SPACE", false},
                           TestCase{0x2007, "FIGURE SPACE", false},

                           // Non-whitespace Unicode
                           TestCase{0x00E9, "LATIN SMALL LETTER E WITH ACUTE",
                           false},
                           TestCase{0x2764, "HEAVY BLACK HEART", false}
                         ));
