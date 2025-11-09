#include "wctype_test_base.h"
#include "mywctype.h"
#include <cwchar>

class PunctTest : public WctypeTest, public ::testing::WithParamInterface<TestCase> {
};

TEST_P(PunctTest, IsPunct) {
  auto test = GetParam();
  int my_result = my_wctype::iswpunct(test.codepoint);
  int std_result = std::iswpunct(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name << ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.name << ")";
}

INSTANTIATE_TEST_SUITE_P(PunctTests, PunctTest, ::testing::Values(
                           // ASCII punctuation
                           TestCase{0x0021, "EXCLAMATION MARK", true},
                           TestCase{0x0022, "QUOTATION MARK", true},
                           TestCase{0x0023, "NUMBER SIGN", true},
                           TestCase{0x002C, "COMMA", true},
                           TestCase{0x002E, "FULL STOP", true},
                           TestCase{0x002F, "SOLIDUS", true},
                           TestCase{0x003A, "COLON", true},
                           TestCase{0x003B, "SEMICOLON", true},
                           TestCase{0x003F, "QUESTION MARK", true},
                           TestCase{0x0040, "COMMERCIAL AT", true},
                           TestCase{0x005B, "LEFT SQUARE BRACKET", true},
                           TestCase{0x005D, "RIGHT SQUARE BRACKET", true},
                           TestCase{0x007B, "LEFT CURLY BRACKET", true},
                           TestCase{0x007D, "RIGHT CURLY BRACKET", true},

                           // ASCII non-punctuation
                           TestCase{0x0041, "LATIN CAPITAL LETTER A", false},
                           TestCase{0x0061, "LATIN SMALL LETTER A", false},
                           TestCase{0x0030, "DIGIT ZERO", false},
                           TestCase{0x0020, "SPACE", false},

                           // Unicode punctuation
                           TestCase{0x00A1, "INVERTED EXCLAMATION MARK", true},
                           TestCase{0x00BF, "INVERTED QUESTION MARK", true},
                           TestCase{0x2013, "EN DASH", true},
                           TestCase{0x2014, "EM DASH", true},
                           TestCase{0x2018, "LEFT SINGLE QUOTATION MARK", true},
                           TestCase{0x2019, "RIGHT SINGLE QUOTATION MARK", true},
                           TestCase{0x201C, "LEFT DOUBLE QUOTATION MARK", true},
                           TestCase{0x201D, "RIGHT DOUBLE QUOTATION MARK", true},
                           TestCase{0x2026, "HORIZONTAL ELLIPSIS", true},
                           TestCase{0x2030, "PER MILLE SIGN", true},
                           TestCase{0x3001, "IDEOGRAPHIC COMMA", true},
                           TestCase{0x3002, "IDEOGRAPHIC FULL STOP", true},
                           TestCase{0xFF01, "FULLWIDTH EXCLAMATION MARK", true},
                           TestCase{0xFF1F, "FULLWIDTH QUESTION MARK", true},

                           // Symbols (treated as punct in C.UTF-8)
                           TestCase{0x00A9, "COPYRIGHT SIGN", true},
                           TestCase{0x20AC, "EURO SIGN", true},
                           TestCase{0x2764, "HEAVY BLACK HEART", true},
                           TestCase{0x00B6, "PILCROW SIGN", true},
                           TestCase{0x00A7, "SECTION SIGN", true},
                           TestCase{0x2022, "BULLET", true},
                           TestCase{0x2023, "TRIANGULAR BULLET", true},
                           TestCase{0x2020, "DAGGER", true},
                           TestCase{0x2021, "DOUBLE DAGGER", true},

                           // Math symbols (should be punct in C.UTF-8)
                           TestCase{0x00D7, "MULTIPLICATION SIGN", true},
                           TestCase{0x00F7, "DIVISION SIGN", true},
                           TestCase{0x2212, "MINUS SIGN", true},
                           TestCase{0x221E, "INFINITY", true}
                         ));
