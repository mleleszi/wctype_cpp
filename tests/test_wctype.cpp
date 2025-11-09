#include <gtest/gtest.h>
#include <cwchar>
#include <locale>
#include "mywctype.h"
#include <wctype.h>

// Test case structure
struct TestCase {
  uint32_t codepoint;
  const char *name;
  bool expected;
};

class WctypeTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    std::locale::global(std::locale("C.UTF-8"));
  }
};

// ==================== ALPHA TESTS ====================
class AlphaTest : public WctypeTest, public ::testing::WithParamInterface<TestCase> {
};

TEST_P(AlphaTest, IsAlpha) {
  auto test = GetParam();
  int my_result = my_wctype::iswalpha(test.codepoint);
  int std_result = std::iswalpha(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name << ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.name << ")";
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
                           TestCase{0x00C0, "LATIN CAPITAL LETTER A WITH GRAVE", true},
                           TestCase{0x00E9, "LATIN SMALL LETTER E WITH ACUTE", true},
                           TestCase{0x00FF, "LATIN SMALL LETTER Y WITH DIAERESIS", true},

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
                           TestCase{0x4E00, "CJK UNIFIED IDEOGRAPH-4E00 (first)", true},
                           TestCase{0x4E01, "CJK UNIFIED IDEOGRAPH-4E01", true},
                           TestCase{0x9FFF, "CJK UNIFIED IDEOGRAPH-9FFF (last in BMP)", true},

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
                           TestCase{0x20000, "CJK UNIFIED IDEOGRAPH-20000", true},

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

// ==================== DIGIT TESTS ====================
class DigitTest : public WctypeTest, public ::testing::WithParamInterface<TestCase> {
};

TEST_P(DigitTest, IsDigit) {
  auto test = GetParam();
  int my_result = my_wctype::iswdigit(test.codepoint);
  int std_result = std::iswdigit(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name << ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.name << ")";
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

// ==================== PUNCT TESTS ====================
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

// ==================== ALNUM TESTS ====================
class AlnumTest : public WctypeTest, public ::testing::WithParamInterface<TestCase> {
};

TEST_P(AlnumTest, IsAlnum) {
  auto test = GetParam();
  int my_result = my_wctype::iswalnum(test.codepoint);
  int std_result = std::iswalnum(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name << ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.name << ")";
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
                           TestCase{0x00E9, "LATIN SMALL LETTER E WITH ACUTE", true},
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

// ==================== BLANK TESTS ====================
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

// ==================== GRAPH TESTS ====================
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

// ==================== LOWER TESTS ====================
class LowerTest : public WctypeTest, public ::testing::WithParamInterface<TestCase> {
};

TEST_P(LowerTest, IsLower) {
  auto test = GetParam();
  int my_result = my_wctype::iswlower(test.codepoint);
  int std_result = std::iswlower(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name << ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.name << ")";
}

INSTANTIATE_TEST_SUITE_P(LowerTests, LowerTest, ::testing::Values(
                           // ASCII lowercase
                           TestCase{0x0061, "LATIN SMALL LETTER A", true},
                           TestCase{0x007A, "LATIN SMALL LETTER Z", true},

                           // ASCII uppercase
                           TestCase{0x0041, "LATIN CAPITAL LETTER A", false},
                           TestCase{0x005A, "LATIN CAPITAL LETTER Z", false},

                           // ASCII non-letters
                           TestCase{0x0030, "DIGIT ZERO", false},
                           TestCase{0x0020, "SPACE", false},
                           TestCase{0x0021, "EXCLAMATION MARK", false},

                           // Latin Extended lowercase
                           TestCase{0x00E0, "LATIN SMALL LETTER A WITH GRAVE", true},
                           TestCase{0x00E9, "LATIN SMALL LETTER E WITH ACUTE", true},
                           TestCase{0x00FF, "LATIN SMALL LETTER Y WITH DIAERESIS", true},

                           // Latin Extended uppercase
                           TestCase{0x00C0, "LATIN CAPITAL LETTER A WITH GRAVE", false},
                           TestCase{0x00C9, "LATIN CAPITAL LETTER E WITH ACUTE", false},

                           // Greek lowercase
                           TestCase{0x03B1, "GREEK SMALL LETTER ALPHA", true},
                           TestCase{0x03C9, "GREEK SMALL LETTER OMEGA", true},

                           // Greek uppercase
                           TestCase{0x0391, "GREEK CAPITAL LETTER ALPHA", false},
                           TestCase{0x03A9, "GREEK CAPITAL LETTER OMEGA", false},

                           // Cyrillic lowercase
                           TestCase{0x0430, "CYRILLIC SMALL LETTER A", true},
                           TestCase{0x044F, "CYRILLIC SMALL LETTER YA", true},

                           // Cyrillic uppercase
                           TestCase{0x0410, "CYRILLIC CAPITAL LETTER A", false},
                           TestCase{0x042F, "CYRILLIC CAPITAL LETTER YA", false},

                           // Caseless scripts (should be false)
                           TestCase{0x05D0, "HEBREW LETTER ALEF", false},
                           TestCase{0x0627, "ARABIC LETTER ALEF", false},
                           TestCase{0x4E00, "CJK UNIFIED IDEOGRAPH-4E00", false}
                         ));

// ==================== SPACE TESTS ====================
class SpaceTest : public WctypeTest, public ::testing::WithParamInterface<TestCase> {
};

TEST_P(SpaceTest, IsSpace) {
  auto test = GetParam();
  int my_result = my_wctype::iswspace(test.codepoint);
  int std_result = std::iswspace(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name << ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.name << ")";
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
                           TestCase{0x00E9, "LATIN SMALL LETTER E WITH ACUTE", false},
                           TestCase{0x2764, "HEAVY BLACK HEART", false}
                         ));
