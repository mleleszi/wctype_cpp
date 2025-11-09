#include <gtest/gtest.h>
#include <cwchar>
#include <locale>
#include "mywctype.h"
#include <wctype.h>

// Test case structures
struct AlphaTestCase {
  uint32_t codepoint;
  const char *name;
  bool should_be_alpha;
};

struct DigitTestCase {
  uint32_t codepoint;
  const char *name;
  bool should_be_digit;
};

struct PunctTestCase {
  uint32_t codepoint;
  const char *name;
  bool should_be_punct;
};

struct AlnumTestCase {
  uint32_t codepoint;
  const char *name;
  bool should_be_alnum;
};

struct BlankTestCase {
  uint32_t codepoint;
  const char *name;
  bool should_be_blank;
};

struct GraphTestCase {
  uint32_t codepoint;
  const char *name;
  bool should_be_graph;
};

struct LowerTestCase {
  uint32_t codepoint;
  const char *name;
  bool should_be_lower;
};

struct SpaceTestCase {
  uint32_t codepoint;
  const char *name;
  bool should_be_space;
};

// Test fixture to set locale
class WctypeTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    // std::locale::global(std::locale("C.UTF-8"));
  }
};

// ==================== ALPHA TESTS ====================
class AlphaTest : public WctypeTest, public ::testing::WithParamInterface<AlphaTestCase> {
};

TEST_P(AlphaTest, IsAlpha) {
  auto test = GetParam();
  int my_result = my_wctype::iswalpha(test.codepoint);
  int std_result = std::iswalpha(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.should_be_alpha)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name << ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.name << ")";
}

INSTANTIATE_TEST_SUITE_P(AlphaTests, AlphaTest, ::testing::Values(
                           AlphaTestCase{0x000000E9, "éééé", true},

                           // ASCII letters
                           AlphaTestCase{0x0041, "LATIN CAPITAL LETTER A", true},
                           AlphaTestCase{0x0061, "LATIN SMALL LETTER A", true},
                           AlphaTestCase{0x005A, "LATIN CAPITAL LETTER Z", true},
                           AlphaTestCase{0x007A, "LATIN SMALL LETTER Z", true},

                           // ASCII non-letters
                           AlphaTestCase{0x0030, "DIGIT ZERO", false},
                           AlphaTestCase{0x0039, "DIGIT NINE", false},
                           AlphaTestCase{0x0020, "SPACE", false},
                           AlphaTestCase{0x0021, "EXCLAMATION MARK", false},
                           AlphaTestCase{0x007E, "TILDE", false},

                           // Latin Extended
                           AlphaTestCase{0x00C0, "LATIN CAPITAL LETTER A WITH GRAVE", true},
                           AlphaTestCase{0x00E9, "LATIN SMALL LETTER E WITH ACUTE", true},
                           AlphaTestCase{0x00FF, "LATIN SMALL LETTER Y WITH DIAERESIS", true},

                           // Greek
                           AlphaTestCase{0x0391, "GREEK CAPITAL LETTER ALPHA", true},
                           AlphaTestCase{0x03B1, "GREEK SMALL LETTER ALPHA", true},
                           AlphaTestCase{0x03C9, "GREEK SMALL LETTER OMEGA", true},

                           // Cyrillic
                           AlphaTestCase{0x0410, "CYRILLIC CAPITAL LETTER A", true},
                           AlphaTestCase{0x0430, "CYRILLIC SMALL LETTER A", true},
                           AlphaTestCase{0x044F, "CYRILLIC SMALL LETTER YA", true},

                           // Arabic
                           AlphaTestCase{0x0627, "ARABIC LETTER ALEF", true},
                           AlphaTestCase{0x0628, "ARABIC LETTER BEH", true},

                           // Hebrew
                           AlphaTestCase{0x05D0, "HEBREW LETTER ALEF", true},
                           AlphaTestCase{0x05D1, "HEBREW LETTER BET", true},

                           // CJK
                           AlphaTestCase{0x4E00, "CJK UNIFIED IDEOGRAPH-4E00 (first)", true},
                           AlphaTestCase{0x4E01, "CJK UNIFIED IDEOGRAPH-4E01", true},
                           AlphaTestCase{0x9FFF, "CJK UNIFIED IDEOGRAPH-9FFF (last in BMP)", true},

                           // Hiragana
                           AlphaTestCase{0x3042, "HIRAGANA LETTER A", true},
                           AlphaTestCase{0x3093, "HIRAGANA LETTER N", true},

                           // Katakana
                           AlphaTestCase{0x30A2, "KATAKANA LETTER A", true},
                           AlphaTestCase{0x30F3, "KATAKANA LETTER N", true},

                           // Hangul
                           AlphaTestCase{0xAC00, "HANGUL SYLLABLE GA", true},
                           AlphaTestCase{0xD7A3, "HANGUL SYLLABLE HIH", true},

                           // Emoji and symbols (not alphabetic)
                           AlphaTestCase{0x1F600, "GRINNING FACE", false},
                           AlphaTestCase{0x2764, "HEAVY BLACK HEART", false},

                           // Special cases
                           AlphaTestCase{0x0000, "NULL", false},
                           AlphaTestCase{0xFFFD, "REPLACEMENT CHARACTER", false},

                           // Beyond BMP - CJK Extension B
                           AlphaTestCase{0x20000, "CJK UNIFIED IDEOGRAPH-20000", true},

                           // Roman numerals - Category Nl (Letter Numbers) - should be ALPHA but NOT DIGIT
                           AlphaTestCase{0x2160, "ROMAN NUMERAL ONE", true},
                           AlphaTestCase{0x2161, "ROMAN NUMERAL TWO", true},
                           AlphaTestCase{0x2162, "ROMAN NUMERAL THREE", true},
                           AlphaTestCase{0x2169, "ROMAN NUMERAL TEN", true},
                           AlphaTestCase{0x216C, "ROMAN NUMERAL FIFTY", true},
                           AlphaTestCase{0x216D, "ROMAN NUMERAL ONE HUNDRED", true},
                           AlphaTestCase{0x216E, "ROMAN NUMERAL FIVE HUNDRED", true},
                           AlphaTestCase{0x216F, "ROMAN NUMERAL ONE THOUSAND", true}
                         ));

