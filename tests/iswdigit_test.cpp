#include "wctype_test_base.h"
#include "mywctype.h"
#include <cwchar>

class DigitTest : public WctypeTest,
                  public ::testing::WithParamInterface<TestCase> {
};

TEST_P(DigitTest, IsDigit) {
  auto test = GetParam();
  int my_result = my_wctype::iswdigit(test.codepoint);
  int std_result = std::iswdigit(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name <<
 ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.
name << ")";
}

INSTANTIATE_TEST_SUITE_P(DigitTests, DigitTest, ::testing::Values(
                           // ASCII digits
                           TestCase{0x0030, "DIGIT ZERO", true},
                           TestCase{0x0031, "DIGIT ONE", true},
                           TestCase{0x0035, "DIGIT FIVE", true},
                           TestCase{0x0039, "DIGIT NINE", true},

                           // ASCII non-digits
                           TestCase{0x0041, "LATIN CAPITAL LETTER A", false},
                           TestCase{0x0061, "LATIN SMALL LETTER A", false},
                           TestCase{0x0020, "SPACE", false},
                           TestCase{0x002E, "FULL STOP", false},
                           TestCase{0x002B, "PLUS SIGN", false},

                           // Unicode digits - not digits in C.UTF-8 (ASCII-centric)
                           TestCase{0x0660, "ARABIC-INDIC DIGIT ZERO", false},
                           TestCase{0x0665, "ARABIC-INDIC DIGIT FIVE", false},
                           TestCase{0x0669, "ARABIC-INDIC DIGIT NINE", false},

                           TestCase{0x0966, "DEVANAGARI DIGIT ZERO", false},
                           TestCase{0x096B, "DEVANAGARI DIGIT FIVE", false},
                           TestCase{0x096F, "DEVANAGARI DIGIT NINE", false},

                           TestCase{0x09E6, "BENGALI DIGIT ZERO", false},
                           TestCase{0x09EF, "BENGALI DIGIT NINE", false},

                           TestCase{0x0E50, "THAI DIGIT ZERO", false},
                           TestCase{0x0E59, "THAI DIGIT NINE", false},

                           TestCase{0x0F20, "TIBETAN DIGIT ZERO", false},
                           TestCase{0x0F29, "TIBETAN DIGIT NINE", false},

                           TestCase{0xFF10, "FULLWIDTH DIGIT ZERO", false},
                           TestCase{0xFF19, "FULLWIDTH DIGIT NINE", false},

                           // Non-decimal numbers (should be false)
                           TestCase{0x2160, "ROMAN NUMERAL ONE", false},
                           TestCase{0x00BD, "VULGAR FRACTION ONE HALF", false},
                           TestCase{0x2081, "SUBSCRIPT ONE", false},
                           TestCase{0x00B2, "SUPERSCRIPT TWO", false},
                           TestCase{0x2170, "SMALL ROMAN NUMERAL ONE", false}
                         ));
