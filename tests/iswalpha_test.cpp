#include "wctype_test_base.h"
#include "mywctype.h"

class AlphaTest : public WctypeTest,
                  public ::testing::WithParamInterface<TestCase> {
};

TEST_P(AlphaTest, IsAlpha) {
  auto test = GetParam();
  int my_result = my_wctype::iswalpha(test.codepoint);
  int std_result = std::iswalpha(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name <<
 ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.
name << ")";
}

INSTANTIATE_TEST_SUITE_P(AlphaTests, AlphaTest, ::testing::Values(
                           TestCase{0x000000E9, "éééé", true},

                           // ASCII letters
                           TestCase{0x0041, "LATIN CAPITAL LETTER A", true},
                           TestCase{0x0061, "LATIN SMALL LETTER A", true},
                           TestCase{0x005A, "LATIN CAPITAL LETTER Z", true},
                           TestCase{0x007A, "LATIN SMALL LETTER Z", true},

                           // ASCII non-letters
                           TestCase{0x0030, "DIGIT ZERO", false},
                           TestCase{0x0039, "DIGIT NINE", false},
                           TestCase{0x0020, "SPACE", false},
                           TestCase{0x0021, "EXCLAMATION MARK", false},
                           TestCase{0x007E, "TILDE", false},

                           // Latin Extended
                           TestCase{0x00C0, "LATIN CAPITAL LETTER A WITH GRAVE",
                           true},
                           TestCase{0x00E9, "LATIN SMALL LETTER E WITH ACUTE",
                           true},
                           TestCase{0x00FF,
                           "LATIN SMALL LETTER Y WITH DIAERESIS", true},

                           // Greek
                           TestCase{0x0391, "GREEK CAPITAL LETTER ALPHA", true},
                           TestCase{0x03B1, "GREEK SMALL LETTER ALPHA", true},
                           TestCase{0x03C9, "GREEK SMALL LETTER OMEGA", true},

                           // Cyrillic
                           TestCase{0x0410, "CYRILLIC CAPITAL LETTER A", true},
                           TestCase{0x0430, "CYRILLIC SMALL LETTER A", true},
                           TestCase{0x044F, "CYRILLIC SMALL LETTER YA", true},

                           // Arabic
                           TestCase{0x0627, "ARABIC LETTER ALEF", true},
                           TestCase{0x0628, "ARABIC LETTER BEH", true},

                           // Hebrew
                           TestCase{0x05D0, "HEBREW LETTER ALEF", true},
                           TestCase{0x05D1, "HEBREW LETTER BET", true},

                           // CJK
                           TestCase{0x4E00, "CJK UNIFIED IDEOGRAPH-4E00 (first)"
                           , true},
                           TestCase{0x4E01, "CJK UNIFIED IDEOGRAPH-4E01", true},
                           TestCase{0x9FFF,
                           "CJK UNIFIED IDEOGRAPH-9FFF (last in BMP)", true},

                           // Hiragana
                           TestCase{0x3042, "HIRAGANA LETTER A", true},
                           TestCase{0x3093, "HIRAGANA LETTER N", true},

                           // Katakana
                           TestCase{0x30A2, "KATAKANA LETTER A", true},
                           TestCase{0x30F3, "KATAKANA LETTER N", true},

                           // Hangul
                           TestCase{0xAC00, "HANGUL SYLLABLE GA", true},
                           TestCase{0xD7A3, "HANGUL SYLLABLE HIH", true},

                           // Emoji and symbols (not alphabetic)
                           TestCase{0x1F600, "GRINNING FACE", false},
                           TestCase{0x2764, "HEAVY BLACK HEART", false},

                           // Special cases
                           TestCase{0x0000, "NULL", false},
                           TestCase{0xFFFD, "REPLACEMENT CHARACTER", false},

                           // Beyond BMP - CJK Extension B
                           TestCase{0x20000, "CJK UNIFIED IDEOGRAPH-20000", true
                           },

                           // Roman numerals - Category Nl (Letter Numbers) - should be ALPHA but NOT DIGIT
                           TestCase{0x2160, "ROMAN NUMERAL ONE", true},
                           TestCase{0x2161, "ROMAN NUMERAL TWO", true},
                           TestCase{0x2162, "ROMAN NUMERAL THREE", true},
                           TestCase{0x2169, "ROMAN NUMERAL TEN", true},
                           TestCase{0x216C, "ROMAN NUMERAL FIFTY", true},
                           TestCase{0x216D, "ROMAN NUMERAL ONE HUNDRED", true},
                           TestCase{0x216E, "ROMAN NUMERAL FIVE HUNDRED", true},
                           TestCase{0x216F, "ROMAN NUMERAL ONE THOUSAND", true}
                         ));
