#include "wctype_test_base.h"
#include "mywctype.h"
#include <cwchar>

class BlankTest : public WctypeTest, public ::testing::WithParamInterface<TestCase> {
};

TEST_P(BlankTest, IsBlank) {
  auto test = GetParam();
  int my_result = my_wctype::iswblank(test.codepoint);
  int std_result = std::iswblank(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name << ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.name << ")";
}

INSTANTIATE_TEST_SUITE_P(BlankTests, BlankTest, ::testing::Values(
                           // Blank characters
                           TestCase{0x0020, "SPACE", true},
                           TestCase{0x0009, "CHARACTER TABULATION (TAB)", true},

                           // Non-blank whitespace
                           TestCase{0x000A, "LINE FEED", false},
                           TestCase{0x000D, "CARRIAGE RETURN", false},
                           TestCase{0x000B, "LINE TABULATION", false},
                           TestCase{0x000C, "FORM FEED", false},

                           // Unicode blank characters
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
                           TestCase{0x3000, "IDEOGRAPHIC SPACE", true},

                           // Non-blank characters
                           TestCase{0x0041, "LATIN CAPITAL LETTER A", false},
                           TestCase{0x0030, "DIGIT ZERO", false},
                           TestCase{0x0021, "EXCLAMATION MARK", false},

                           TestCase{0x00A0, "NO-BREAK SPACE", false},
                           TestCase{0x2007, "FIGURE SPACE", false},
                           TestCase{0x202F, "NARROW NO-BREAK SPACE", false},
                           TestCase{0x205F, "MEDIUM MATHEMATICAL SPACE", true}
                         ));
