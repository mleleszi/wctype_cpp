#include "wctype_test_base.h"
#include "mywctype.h"

class AlnumTest : public WctypeTest,
                  public ::testing::WithParamInterface<TestCase> {
};

TEST_P(AlnumTest, IsAlnum) {
  auto test = GetParam();
  int my_result = my_wctype::iswalnum(test.codepoint);
  int std_result = std::iswalnum(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name <<
 ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.
name << ")";
}

INSTANTIATE_TEST_SUITE_P(AlnumTests, AlnumTest, ::testing::Values(
                           // ASCII alphanumeric
                           TestCase{0x0041, "LATIN CAPITAL LETTER A", true},
                           TestCase{0x0061, "LATIN SMALL LETTER A", true},
                           TestCase{0x005A, "LATIN CAPITAL LETTER Z", true},
                           TestCase{0x007A, "LATIN SMALL LETTER Z", true},
                           TestCase{0x0030, "DIGIT ZERO", true},
                           TestCase{0x0039, "DIGIT NINE", true},

                           // ASCII non-alphanumeric
                           TestCase{0x0020, "SPACE", false},
                           TestCase{0x0021, "EXCLAMATION MARK", false},
                           TestCase{0x002E, "FULL STOP", false},
                           TestCase{0x0040, "COMMERCIAL AT", false},

                           // Unicode letters
                           TestCase{0x00E9, "LATIN SMALL LETTER E WITH ACUTE",
                           true},
                           TestCase{0x0391, "GREEK CAPITAL LETTER ALPHA", true},
                           TestCase{0x0430, "CYRILLIC SMALL LETTER A", true},

                           // Unicode digits
                           TestCase{0x0660, "ARABIC-INDIC DIGIT ZERO", true},
                           TestCase{0x0966, "DEVANAGARI DIGIT ZERO", true},

                           // CJK
                           TestCase{0x4E00, "CJK UNIFIED IDEOGRAPH-4E00", true},
                           TestCase{0x3042, "HIRAGANA LETTER A", true},

                           // Non-alphanumeric
                           TestCase{0x2764, "HEAVY BLACK HEART", false},
                           TestCase{0x0000, "NULL", false}
                         ));
