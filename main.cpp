#include <iostream>

#include <wctype_properties.h>
#include <wctype_table.h>

#include "wctype_table.h"
#include <iostream>
#include <iomanip>
#include <cwchar>
#include <locale>
#include <string>
#include <vector>
#include <chrono>
#include <wchar.h>


// Our implementation
namespace my_wctype {
  int iswalpha(wchar_t wc) {
    // Fast path: ASCII
    if (wc <= 0x7F) {
      return (wc >= 'A' && wc <= 'Z') || (wc >= 'a' && wc <= 'z');
    }

    // Fast path: Common CJK ranges (all alphabetic)
    if ((wc >= 0x4E00 && wc <= 0x9FFF) || // CJK Unified Ideographs
        (wc >= 0x3400 && wc <= 0x4DBF) || // CJK Extension A
        (wc >= 0x20000 && wc <= 0x2A6DF)) {
      // CJK Extension B
      return 1;
    }

    // Table lookup
    return lookup_properties(wc) & PROP_ALPHA;
  }

  int iswdigit(wchar_t wc) {
    return lookup_properties(wc) & PROP_DIGIT;
  }

  int iswpunct(wchar_t wc) {
    return lookup_properties(wc) & PROP_PUNCT;
  }

  int iswalnum(wchar_t wc) {
    return lookup_properties(wc) & (PROP_ALPHA | PROP_DIGIT);
  }

  int iswblank(wchar_t wc) {
    return lookup_properties(wc) & PROP_BLANK;
  }

  int iswgraph(wchar_t wc) {
    return lookup_properties(wc) & PROP_GRAPH;
  }

  int iswlower(wchar_t wc) {
    return lookup_properties(wc) & PROP_LOWER;
  }

  int iswspace(wchar_t wc) {
    return lookup_properties(wc) & PROP_SPACE;
  }
} // namespace my_wctype

// Test case structures
struct TestCase {
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

std::vector<TestCase> tests = {
  {0x000000E9, "éééé", true},

  // ASCII letters
  {0x0041, "LATIN CAPITAL LETTER A", true},
  {0x0061, "LATIN SMALL LETTER A", true},
  {0x005A, "LATIN CAPITAL LETTER Z", true},
  {0x007A, "LATIN SMALL LETTER Z", true},

  // ASCII non-letters
  {0x0030, "DIGIT ZERO", false},
  {0x0039, "DIGIT NINE", false},
  {0x0020, "SPACE", false},
  {0x0021, "EXCLAMATION MARK", false},
  {0x007E, "TILDE", false},

  // Latin Extended
  {0x00C0, "LATIN CAPITAL LETTER A WITH GRAVE", true},
  {0x00E9, "LATIN SMALL LETTER E WITH ACUTE", true},
  {0x00FF, "LATIN SMALL LETTER Y WITH DIAERESIS", true},

  // Greek
  {0x0391, "GREEK CAPITAL LETTER ALPHA", true},
  {0x03B1, "GREEK SMALL LETTER ALPHA", true},
  {0x03C9, "GREEK SMALL LETTER OMEGA", true},

  // Cyrillic
  {0x0410, "CYRILLIC CAPITAL LETTER A", true},
  {0x0430, "CYRILLIC SMALL LETTER A", true},
  {0x044F, "CYRILLIC SMALL LETTER YA", true},

  // Arabic
  {0x0627, "ARABIC LETTER ALEF", true},
  {0x0628, "ARABIC LETTER BEH", true},

  // Hebrew
  {0x05D0, "HEBREW LETTER ALEF", true},
  {0x05D1, "HEBREW LETTER BET", true},

  // CJK
  {0x4E00, "CJK UNIFIED IDEOGRAPH-4E00 (first)", true},
  {0x4E01, "CJK UNIFIED IDEOGRAPH-4E01", true},
  {0x9FFF, "CJK UNIFIED IDEOGRAPH-9FFF (last in BMP)", true},

  // Hiragana
  {0x3042, "HIRAGANA LETTER A", true},
  {0x3093, "HIRAGANA LETTER N", true},

  // Katakana
  {0x30A2, "KATAKANA LETTER A", true},
  {0x30F3, "KATAKANA LETTER N", true},

  // Hangul
  {0xAC00, "HANGUL SYLLABLE GA", true},
  {0xD7A3, "HANGUL SYLLABLE HIH", true},

  // Emoji and symbols (not alphabetic)
  {0x1F600, "GRINNING FACE", false},
  {0x2764, "HEAVY BLACK HEART", false},

  // Special cases
  {0x0000, "NULL", false},
  {0xFFFD, "REPLACEMENT CHARACTER", false},

  // Beyond BMP - CJK Extension B
  {0x20000, "CJK UNIFIED IDEOGRAPH-20000", true},

  // Roman numerals - Category Nl (Letter Numbers) - should be ALPHA but NOT DIGIT
  {0x2160, "ROMAN NUMERAL ONE", true}, // Ⅰ - iswalpha
  {0x2161, "ROMAN NUMERAL TWO", true}, // Ⅱ - iswalpha
  {0x2162, "ROMAN NUMERAL THREE", true}, // Ⅲ - iswalpha
  {0x2169, "ROMAN NUMERAL TEN", true}, // Ⅹ - iswalpha
  {0x216C, "ROMAN NUMERAL FIFTY", true}, // Ⅼ - iswalpha
  {0x216D, "ROMAN NUMERAL ONE HUNDRED", true}, // Ⅽ - iswalpha
  {0x216E, "ROMAN NUMERAL FIVE HUNDRED", true}, // Ⅾ - iswalpha
  {0x216F, "ROMAN NUMERAL ONE THOUSAND", true}, // Ⅿ - iswalpha
};


std::vector<DigitTestCase> digit_tests = {
  // ASCII digits
  {0x0030, "DIGIT ZERO", true},
  {0x0031, "DIGIT ONE", true},
  {0x0035, "DIGIT FIVE", true},
  {0x0039, "DIGIT NINE", true},

  // ASCII non-digits
  {0x0041, "LATIN CAPITAL LETTER A", false},
  {0x0061, "LATIN SMALL LETTER A", false},
  {0x0020, "SPACE", false},
  {0x002E, "FULL STOP", false},
  {0x002B, "PLUS SIGN", false},

  // These are unicode digits, but not c.utf8, c.utf8 is ascii centric
  // Unicode digits - Arabic-Indic
  {0x0660, "ARABIC-INDIC DIGIT ZERO", false},
  {0x0665, "ARABIC-INDIC DIGIT FIVE", false},
  {0x0669, "ARABIC-INDIC DIGIT NINE", false},

  // Devanagari digits
  {0x0966, "DEVANAGARI DIGIT ZERO", false},
  {0x096B, "DEVANAGARI DIGIT FIVE", false},
  {0x096F, "DEVANAGARI DIGIT NINE", false},

  // Bengali digits
  {0x09E6, "BENGALI DIGIT ZERO", false},
  {0x09EF, "BENGALI DIGIT NINE", false},

  // Thai digits
  {0x0E50, "THAI DIGIT ZERO", false},
  {0x0E59, "THAI DIGIT NINE", false},

  // Tibetan digits
  {0x0F20, "TIBETAN DIGIT ZERO", false},
  {0x0F29, "TIBETAN DIGIT NINE", false},

  // Fullwidth digits
  {0xFF10, "FULLWIDTH DIGIT ZERO", false},
  {0xFF19, "FULLWIDTH DIGIT NINE", false},

  // Non-decimal numbers (should be false)
  {0x2160, "ROMAN NUMERAL ONE", false},
  {0x00BD, "VULGAR FRACTION ONE HALF", false},
  {0x2081, "SUBSCRIPT ONE", false},
  {0x00B2, "SUPERSCRIPT TWO", false},

  // For iswdigit - Roman numerals should be FALSE
  {0x2160, "ROMAN NUMERAL ONE", false}, // Ⅰ - NOT iswdigit
  {0x2161, "ROMAN NUMERAL TWO", false}, // Ⅱ - NOT iswdigit
  {0x2170, "SMALL ROMAN NUMERAL ONE", false}, // ⅰ - NOT iswdigit
};

std::vector<PunctTestCase> punct_tests = {
  // ASCII punctuation
  {0x0021, "EXCLAMATION MARK", true},
  {0x0022, "QUOTATION MARK", true},
  {0x0023, "NUMBER SIGN", true},
  {0x002C, "COMMA", true},
  {0x002E, "FULL STOP", true},
  {0x002F, "SOLIDUS", true},
  {0x003A, "COLON", true},
  {0x003B, "SEMICOLON", true},
  {0x003F, "QUESTION MARK", true},
  {0x0040, "COMMERCIAL AT", true},
  {0x005B, "LEFT SQUARE BRACKET", true},
  {0x005D, "RIGHT SQUARE BRACKET", true},
  {0x007B, "LEFT CURLY BRACKET", true},
  {0x007D, "RIGHT CURLY BRACKET", true},

  // ASCII non-punctuation
  {0x0041, "LATIN CAPITAL LETTER A", false},
  {0x0061, "LATIN SMALL LETTER A", false},
  {0x0030, "DIGIT ZERO", false},
  {0x0020, "SPACE", false},

  // Unicode punctuation
  {0x00A1, "INVERTED EXCLAMATION MARK", true},
  {0x00BF, "INVERTED QUESTION MARK", true},
  {0x2013, "EN DASH", true},
  {0x2014, "EM DASH", true},
  {0x2018, "LEFT SINGLE QUOTATION MARK", true},
  {0x2019, "RIGHT SINGLE QUOTATION MARK", true},
  {0x201C, "LEFT DOUBLE QUOTATION MARK", true},
  {0x201D, "RIGHT DOUBLE QUOTATION MARK", true},
  {0x2026, "HORIZONTAL ELLIPSIS", true},
  {0x2030, "PER MILLE SIGN", true},
  {0x3001, "IDEOGRAPHIC COMMA", true},
  {0x3002, "IDEOGRAPHIC FULL STOP", true},
  {0xFF01, "FULLWIDTH EXCLAMATION MARK", true},
  {0xFF1F, "FULLWIDTH QUESTION MARK", true},

  // Non-punctuation characters, but are in c.utf8?
  {0x00A9, "COPYRIGHT SIGN", true},
  {0x20AC, "EURO SIGN", true},
  {0x2764, "HEAVY BLACK HEART", true},

  {0x00B6, "PILCROW SIGN", true},
  {0x00A7, "SECTION SIGN", true},
  {0x2022, "BULLET", true},
  {0x2023, "TRIANGULAR BULLET", true},
  {0x2020, "DAGGER", true},
  {0x2021, "DOUBLE DAGGER", true},

  // Math symbols (should be punct in C.UTF-8)
  {0x00D7, "MULTIPLICATION SIGN", true},
  {0x00F7, "DIVISION SIGN", true},
  {0x2212, "MINUS SIGN", true},
  {0x221E, "INFINITY", true},
};

std::vector<AlnumTestCase> alnum_tests = {
  // ASCII alphanumeric
  {0x0041, "LATIN CAPITAL LETTER A", true},
  {0x0061, "LATIN SMALL LETTER A", true},
  {0x005A, "LATIN CAPITAL LETTER Z", true},
  {0x007A, "LATIN SMALL LETTER Z", true},
  {0x0030, "DIGIT ZERO", true},
  {0x0039, "DIGIT NINE", true},

  // ASCII non-alphanumeric
  {0x0020, "SPACE", false},
  {0x0021, "EXCLAMATION MARK", false},
  {0x002E, "FULL STOP", false},
  {0x0040, "COMMERCIAL AT", false},

  // Unicode letters
  {0x00E9, "LATIN SMALL LETTER E WITH ACUTE", true},
  {0x0391, "GREEK CAPITAL LETTER ALPHA", true},
  {0x0430, "CYRILLIC SMALL LETTER A", true},

  // Unicode digits
  {0x0660, "ARABIC-INDIC DIGIT ZERO", true},
  {0x0966, "DEVANAGARI DIGIT ZERO", true},

  // CJK
  {0x4E00, "CJK UNIFIED IDEOGRAPH-4E00", true},
  {0x3042, "HIRAGANA LETTER A", true},

  // Non-alphanumeric
  {0x2764, "HEAVY BLACK HEART", false},
  {0x0000, "NULL", false},
};


std::vector<BlankTestCase> blank_tests = {
  // Blank characters
  {0x0020, "SPACE", true},
  {0x0009, "CHARACTER TABULATION (TAB)", true},

  // Non-blank whitespace
  {0x000A, "LINE FEED", false},
  {0x000D, "CARRIAGE RETURN", false},
  {0x000B, "LINE TABULATION", false},
  {0x000C, "FORM FEED", false},

  // Unicode blank characters
  {0x1680, "OGHAM SPACE MARK", true},
  {0x2000, "EN QUAD", true},
  {0x2001, "EM QUAD", true},
  {0x2002, "EN SPACE", true},
  {0x2003, "EM SPACE", true},
  {0x2004, "THREE-PER-EM SPACE", true},
  {0x2005, "FOUR-PER-EM SPACE", true},
  {0x2006, "SIX-PER-EM SPACE", true},
  {0x2008, "PUNCTUATION SPACE", true},
  {0x2009, "THIN SPACE", true},
  {0x200A, "HAIR SPACE", true},
  {0x3000, "IDEOGRAPHIC SPACE", true},

  // Non-blank characters
  {0x0041, "LATIN CAPITAL LETTER A", false},
  {0x0030, "DIGIT ZERO", false},
  {0x0021, "EXCLAMATION MARK", false},

  {0x00A0, "NO-BREAK SPACE", false}, // NOT blank in C.UTF-8
  {0x2007, "FIGURE SPACE", false}, // NOT blank
  {0x202F, "NARROW NO-BREAK SPACE", false}, // NOT blank
  {0x205F, "MEDIUM MATHEMATICAL SPACE", true},
};

std::vector<GraphTestCase> graph_tests = {
  // Graphical characters (visible)
  {0x0041, "LATIN CAPITAL LETTER A", true},
  {0x0061, "LATIN SMALL LETTER A", true},
  {0x0030, "DIGIT ZERO", true},
  {0x0021, "EXCLAMATION MARK", true},
  {0x007E, "TILDE", true},

  // Non-graphical (whitespace and control)
  {0x0020, "SPACE", false},
  {0x0009, "CHARACTER TABULATION", false},
  {0x000A, "LINE FEED", false},
  {0x000D, "CARRIAGE RETURN", false},
  {0x0000, "NULL", false},
  {0x001F, "INFORMATION SEPARATOR ONE", false},

  // Unicode graphical
  {0x00E9, "LATIN SMALL LETTER E WITH ACUTE", true},
  {0x0391, "GREEK CAPITAL LETTER ALPHA", true},
  {0x4E00, "CJK UNIFIED IDEOGRAPH-4E00", true},
  {0x2764, "HEAVY BLACK HEART", true},
  {0x1F600, "GRINNING FACE", true},

  // Unicode non-graphical
  {0x00A0, "NO-BREAK SPACE", true}, // special case
  {0x3000, "IDEOGRAPHIC SPACE", false},
};

std::vector<LowerTestCase> lower_tests = {
  // ASCII lowercase
  {0x0061, "LATIN SMALL LETTER A", true},
  {0x007A, "LATIN SMALL LETTER Z", true},

  // ASCII uppercase
  {0x0041, "LATIN CAPITAL LETTER A", false},
  {0x005A, "LATIN CAPITAL LETTER Z", false},

  // ASCII non-letters
  {0x0030, "DIGIT ZERO", false},
  {0x0020, "SPACE", false},
  {0x0021, "EXCLAMATION MARK", false},

  // Latin Extended lowercase
  {0x00E0, "LATIN SMALL LETTER A WITH GRAVE", true},
  {0x00E9, "LATIN SMALL LETTER E WITH ACUTE", true},
  {0x00FF, "LATIN SMALL LETTER Y WITH DIAERESIS", true},

  // Latin Extended uppercase
  {0x00C0, "LATIN CAPITAL LETTER A WITH GRAVE", false},
  {0x00C9, "LATIN CAPITAL LETTER E WITH ACUTE", false},

  // Greek lowercase
  {0x03B1, "GREEK SMALL LETTER ALPHA", true},
  {0x03C9, "GREEK SMALL LETTER OMEGA", true},

  // Greek uppercase
  {0x0391, "GREEK CAPITAL LETTER ALPHA", false},
  {0x03A9, "GREEK CAPITAL LETTER OMEGA", false},

  // Cyrillic lowercase
  {0x0430, "CYRILLIC SMALL LETTER A", true},
  {0x044F, "CYRILLIC SMALL LETTER YA", true},

  // Cyrillic uppercase
  {0x0410, "CYRILLIC CAPITAL LETTER A", false},
  {0x042F, "CYRILLIC CAPITAL LETTER YA", false},

  // Caseless scripts (should be false)
  {0x05D0, "HEBREW LETTER ALEF", false},
  {0x0627, "ARABIC LETTER ALEF", false},
  {0x4E00, "CJK UNIFIED IDEOGRAPH-4E00", false},
};

std::vector<SpaceTestCase> space_tests = {
  // ASCII whitespace
  {0x0020, "SPACE", true},
  {0x0009, "CHARACTER TABULATION (TAB)", true},
  {0x000A, "LINE FEED", true},
  {0x000B, "LINE TABULATION", true},
  {0x000C, "FORM FEED", true},
  {0x000D, "CARRIAGE RETURN", true},

  // ASCII non-whitespace
  {0x0041, "LATIN CAPITAL LETTER A", false},
  {0x0030, "DIGIT ZERO", false},
  {0x0021, "EXCLAMATION MARK", false},

  // Unicode whitespace
  {0x1680, "OGHAM SPACE MARK", true},
  {0x2000, "EN QUAD", true},
  {0x2001, "EM QUAD", true},
  {0x2002, "EN SPACE", true},
  {0x2003, "EM SPACE", true},
  {0x2004, "THREE-PER-EM SPACE", true},
  {0x2005, "FOUR-PER-EM SPACE", true},
  {0x2006, "SIX-PER-EM SPACE", true},
  {0x2008, "PUNCTUATION SPACE", true},
  {0x2009, "THIN SPACE", true},
  {0x200A, "HAIR SPACE", true},
  {0x2028, "LINE SEPARATOR", true},
  {0x2029, "PARAGRAPH SEPARATOR", true},
  {0x205F, "MEDIUM MATHEMATICAL SPACE", true},
  {0x3000, "IDEOGRAPHIC SPACE", true},

  {0x202F, "NARROW NO-BREAK SPACE", false},
  {0x0085, "NEXT LINE", false},
  {0x00A0, "NO-BREAK SPACE", false},
  {0x2007, "FIGURE SPACE", false},


  // Non-whitespace Unicode
  {0x00E9, "LATIN SMALL LETTER E WITH ACUTE", false},
  {0x2764, "HEAVY BLACK HEART", false},
};

int main() {
  std::cout << "=== wctype iswalpha() Implementation Test ===\n\n";

  std::locale::global(std::locale("C.UTF-8"));
  // Test cases

  int passed = 0;
  int failed = 0;

  std::cout << std::left;
  std::cout << std::setw(10) << "Codepoint"
      << std::setw(40) << "Character/Name"
      << std::setw(10) << "Expected"
      << std::setw(8) << "My"
      << std::setw(8) << "Std"
      << "Result\n";
  std::cout << std::string(85, '-') << "\n";

  for (const auto &test: tests) {
    int my_result = my_wctype::iswalpha(test.codepoint);
    int std_result = std::iswalpha(static_cast<wint_t>(test.codepoint));
    bool my_correct = (my_result != 0) == test.should_be_alpha;
    bool std_correct = (std_result != 0) == test.should_be_alpha;
    bool results_match = (my_result != 0) == (std_result != 0);

    std::cout << "U+" << std::hex << std::setfill('0') << std::setw(4)
        << std::uppercase << test.codepoint << std::dec << std::setfill(' ')
        << "  ";

    std::cout << std::setw(40) << test.name
        << std::setw(10) << (test.should_be_alpha ? "alpha" : "not-alpha")
        << std::setw(8) << (my_result ? "alpha" : "not")
        << std::setw(8) << (std_result ? "alpha" : "not");

    if (my_correct && results_match) {
      std::cout << "✓ PASS";
      passed++;
    } else {
      std::cout << "✗ FAIL";
      if (!results_match) std::cout << " (mismatch with std)";
      failed++;
    }
    std::cout << "\n";
  }

  std::cout << "\n" << std::string(85, '-') << "\n";
  std::cout << "Results: " << passed << " passed, " << failed << " failed\n";

  if (failed == 0) {
    std::cout << "\n✓ All tests passed!\n";
  } else {
    std::cout << "\n✗ Some tests failed!\n";
  }

  // Test iswdigit
  std::cout << "\n\n=== wctype iswdigit() Implementation Test ===\n\n";


  int digit_passed = 0;
  int digit_failed = 0;

  std::cout << std::left;
  std::cout << std::setw(10) << "Codepoint"
      << std::setw(40) << "Character/Name"
      << std::setw(10) << "Expected"
      << std::setw(8) << "My"
      << std::setw(8) << "Std"
      << "Result\n";
  std::cout << std::string(85, '-') << "\n";

  for (const auto &test: digit_tests) {
    int my_result = my_wctype::iswdigit(test.codepoint);
    int std_result = std::iswdigit(static_cast<wint_t>(test.codepoint));
    bool my_correct = (my_result != 0) == test.should_be_digit;
    bool results_match = (my_result != 0) == (std_result != 0);

    std::cout << "U+" << std::hex << std::setfill('0') << std::setw(4)
        << std::uppercase << test.codepoint << std::dec << std::setfill(' ')
        << "  ";

    std::cout << std::setw(40) << test.name
        << std::setw(10) << (test.should_be_digit ? "digit" : "not-digit")
        << std::setw(8) << (my_result ? "digit" : "not")
        << std::setw(8) << (std_result ? "digit" : "not");

    if (my_correct && results_match) {
      std::cout << "✓ PASS";
      digit_passed++;
    } else {
      std::cout << "✗ FAIL";
      if (!results_match) std::cout << " (mismatch with std)";
      digit_failed++;
    }
    std::cout << "\n";
  }

  std::cout << "\n" << std::string(85, '-') << "\n";
  std::cout << "Results: " << digit_passed << " passed, " << digit_failed << " failed\n";

  if (digit_failed == 0) {
    std::cout << "\n✓ All digit tests passed!\n";
  } else {
    std::cout << "\n✗ Some digit tests failed!\n";
  }

  // Test iswpunct
  std::cout << "\n\n=== wctype iswpunct() Implementation Test ===\n\n";


  int punct_passed = 0;
  int punct_failed = 0;

  std::cout << std::left;
  std::cout << std::setw(10) << "Codepoint"
      << std::setw(40) << "Character/Name"
      << std::setw(10) << "Expected"
      << std::setw(8) << "My"
      << std::setw(8) << "Std"
      << "Result\n";
  std::cout << std::string(85, '-') << "\n";

  for (const auto &test: punct_tests) {
    int my_result = my_wctype::iswpunct(test.codepoint);
    int std_result = std::iswpunct(static_cast<wint_t>(test.codepoint));
    bool my_correct = (my_result != 0) == test.should_be_punct;
    bool results_match = (my_result != 0) == (std_result != 0);

    std::cout << "U+" << std::hex << std::setfill('0') << std::setw(4)
        << std::uppercase << test.codepoint << std::dec << std::setfill(' ')
        << "  ";

    std::cout << std::setw(40) << test.name
        << std::setw(10) << (test.should_be_punct ? "punct" : "not-punct")
        << std::setw(8) << (my_result ? "punct" : "not")
        << std::setw(8) << (std_result ? "punct" : "not");

    if (my_correct && results_match) {
      std::cout << "✓ PASS";
      punct_passed++;
    } else {
      std::cout << "✗ FAIL";
      if (!results_match) std::cout << " (mismatch with std)";
      punct_failed++;
    }
    std::cout << "\n";
  }

  std::cout << "\n" << std::string(85, '-') << "\n";
  std::cout << "Results: " << punct_passed << " passed, " << punct_failed << " failed\n";

  if (punct_failed == 0) {
    std::cout << "\n✓ All punctuation tests passed!\n";
  } else {
    std::cout << "\n✗ Some punctuation tests failed!\n";
  }

  // Test iswalnum
  std::cout << "\n\n=== wctype iswalnum() Implementation Test ===\n\n";


  int alnum_passed = 0;
  int alnum_failed = 0;

  std::cout << std::left;
  std::cout << std::setw(10) << "Codepoint"
      << std::setw(40) << "Character/Name"
      << std::setw(10) << "Expected"
      << std::setw(8) << "My"
      << std::setw(8) << "Std"
      << "Result\n";
  std::cout << std::string(85, '-') << "\n";

  for (const auto &test: alnum_tests) {
    int my_result = my_wctype::iswalnum(test.codepoint);
    int std_result = std::iswalnum(static_cast<wint_t>(test.codepoint));
    bool my_correct = (my_result != 0) == test.should_be_alnum;
    bool results_match = (my_result != 0) == (std_result != 0);

    std::cout << "U+" << std::hex << std::setfill('0') << std::setw(4)
        << std::uppercase << test.codepoint << std::dec << std::setfill(' ')
        << "  ";

    std::cout << std::setw(40) << test.name
        << std::setw(10) << (test.should_be_alnum ? "alnum" : "not-alnum")
        << std::setw(8) << (my_result ? "alnum" : "not")
        << std::setw(8) << (std_result ? "alnum" : "not");

    if (my_correct && results_match) {
      std::cout << "✓ PASS";
      alnum_passed++;
    } else {
      std::cout << "✗ FAIL";
      if (!results_match) std::cout << " (mismatch with std)";
      alnum_failed++;
    }
    std::cout << "\n";
  }

  std::cout << "\n" << std::string(85, '-') << "\n";
  std::cout << "Results: " << alnum_passed << " passed, " << alnum_failed << " failed\n";

  if (alnum_failed == 0) {
    std::cout << "\n✓ All alnum tests passed!\n";
  } else {
    std::cout << "\n✗ Some alnum tests failed!\n";
  }

  // Test iswblank
  std::cout << "\n\n=== wctype iswblank() Implementation Test ===\n\n";


  int blank_passed = 0;
  int blank_failed = 0;

  std::cout << std::left;
  std::cout << std::setw(10) << "Codepoint"
      << std::setw(40) << "Character/Name"
      << std::setw(10) << "Expected"
      << std::setw(8) << "My"
      << std::setw(8) << "Std"
      << "Result\n";
  std::cout << std::string(85, '-') << "\n";

  for (const auto &test: blank_tests) {
    int my_result = my_wctype::iswblank(test.codepoint);
    int std_result = std::iswblank(static_cast<wint_t>(test.codepoint));
    bool my_correct = (my_result != 0) == test.should_be_blank;
    bool results_match = (my_result != 0) == (std_result != 0);

    std::cout << "U+" << std::hex << std::setfill('0') << std::setw(4)
        << std::uppercase << test.codepoint << std::dec << std::setfill(' ')
        << "  ";

    std::cout << std::setw(40) << test.name
        << std::setw(10) << (test.should_be_blank ? "blank" : "not-blank")
        << std::setw(8) << (my_result ? "blank" : "not")
        << std::setw(8) << (std_result ? "blank" : "not");

    if (my_correct && results_match) {
      std::cout << "✓ PASS";
      blank_passed++;
    } else {
      std::cout << "✗ FAIL";
      if (!results_match) std::cout << " (mismatch with std)";
      blank_failed++;
    }
    std::cout << "\n";
  }

  std::cout << "\n" << std::string(85, '-') << "\n";
  std::cout << "Results: " << blank_passed << " passed, " << blank_failed << " failed\n";

  if (blank_failed == 0) {
    std::cout << "\n✓ All blank tests passed!\n";
  } else {
    std::cout << "\n✗ Some blank tests failed!\n";
  }

  // Test iswgraph
  std::cout << "\n\n=== wctype iswgraph() Implementation Test ===\n\n";


  int graph_passed = 0;
  int graph_failed = 0;

  std::cout << std::left;
  std::cout << std::setw(10) << "Codepoint"
      << std::setw(40) << "Character/Name"
      << std::setw(10) << "Expected"
      << std::setw(8) << "My"
      << std::setw(8) << "Std"
      << "Result\n";
  std::cout << std::string(85, '-') << "\n";

  for (const auto &test: graph_tests) {
    int my_result = my_wctype::iswgraph(test.codepoint);
    int std_result = std::iswgraph(static_cast<wint_t>(test.codepoint));
    bool my_correct = (my_result != 0) == test.should_be_graph;
    bool results_match = (my_result != 0) == (std_result != 0);

    std::cout << "U+" << std::hex << std::setfill('0') << std::setw(4)
        << std::uppercase << test.codepoint << std::dec << std::setfill(' ')
        << "  ";

    std::cout << std::setw(40) << test.name
        << std::setw(10) << (test.should_be_graph ? "graph" : "not-graph")
        << std::setw(8) << (my_result ? "graph" : "not")
        << std::setw(8) << (std_result ? "graph" : "not");

    if (my_correct && results_match) {
      std::cout << "✓ PASS";
      graph_passed++;
    } else {
      std::cout << "✗ FAIL";
      if (!results_match) std::cout << " (mismatch with std)";
      graph_failed++;
    }
    std::cout << "\n";
  }

  std::cout << "\n" << std::string(85, '-') << "\n";
  std::cout << "Results: " << graph_passed << " passed, " << graph_failed << " failed\n";

  if (graph_failed == 0) {
    std::cout << "\n✓ All graph tests passed!\n";
  } else {
    std::cout << "\n✗ Some graph tests failed!\n";
  }

  // Test iswlower
  std::cout << "\n\n=== wctype iswlower() Implementation Test ===\n\n";


  int lower_passed = 0;
  int lower_failed = 0;

  std::cout << std::left;
  std::cout << std::setw(10) << "Codepoint"
      << std::setw(40) << "Character/Name"
      << std::setw(10) << "Expected"
      << std::setw(8) << "My"
      << std::setw(8) << "Std"
      << "Result\n";
  std::cout << std::string(85, '-') << "\n";

  for (const auto &test: lower_tests) {
    int my_result = my_wctype::iswlower(test.codepoint);
    int std_result = std::iswlower(static_cast<wint_t>(test.codepoint));
    bool my_correct = (my_result != 0) == test.should_be_lower;
    bool results_match = (my_result != 0) == (std_result != 0);

    std::cout << "U+" << std::hex << std::setfill('0') << std::setw(4)
        << std::uppercase << test.codepoint << std::dec << std::setfill(' ')
        << "  ";

    std::cout << std::setw(40) << test.name
        << std::setw(10) << (test.should_be_lower ? "lower" : "not-lower")
        << std::setw(8) << (my_result ? "lower" : "not")
        << std::setw(8) << (std_result ? "lower" : "not");

    if (my_correct && results_match) {
      std::cout << "✓ PASS";
      lower_passed++;
    } else {
      std::cout << "✗ FAIL";
      if (!results_match) std::cout << " (mismatch with std)";
      lower_failed++;
    }
    std::cout << "\n";
  }

  std::cout << "\n" << std::string(85, '-') << "\n";
  std::cout << "Results: " << lower_passed << " passed, " << lower_failed << " failed\n";

  if (lower_failed == 0) {
    std::cout << "\n✓ All lower tests passed!\n";
  } else {
    std::cout << "\n✗ Some lower tests failed!\n";
  }

  // Test iswspace
  std::cout << "\n\n=== wctype iswspace() Implementation Test ===\n\n";


  int space_passed = 0;
  int space_failed = 0;

  std::cout << std::left;
  std::cout << std::setw(10) << "Codepoint"
      << std::setw(40) << "Character/Name"
      << std::setw(10) << "Expected"
      << std::setw(8) << "My"
      << std::setw(8) << "Std"
      << "Result\n";
  std::cout << std::string(85, '-') << "\n";

  for (const auto &test: space_tests) {
    int my_result = my_wctype::iswspace(test.codepoint);
    int std_result = std::iswspace(static_cast<wint_t>(test.codepoint));
    bool my_correct = (my_result != 0) == test.should_be_space;
    bool results_match = (my_result != 0) == (std_result != 0);

    std::cout << "U+" << std::hex << std::setfill('0') << std::setw(4)
        << std::uppercase << test.codepoint << std::dec << std::setfill(' ')
        << "  ";

    std::cout << std::setw(40) << test.name
        << std::setw(10) << (test.should_be_space ? "space" : "not-space")
        << std::setw(8) << (my_result ? "space" : "not")
        << std::setw(8) << (std_result ? "space" : "not");

    if (my_correct && results_match) {
      std::cout << "✓ PASS";
      space_passed++;
    } else {
      std::cout << "✗ FAIL";
      if (!results_match) std::cout << " (mismatch with std)";
      space_failed++;
    }
    std::cout << "\n";
  }

  std::cout << "\n" << std::string(85, '-') << "\n";
  std::cout << "Results: " << space_passed << " passed, " << space_failed << " failed\n";

  if (space_failed == 0) {
    std::cout << "\n✓ All space tests passed!\n";
  } else {
    std::cout << "\n✗ Some space tests failed!\n";
  }

  // Update overall results
  passed += digit_passed + punct_passed + alnum_passed + blank_passed + graph_passed + lower_passed + space_passed;
  failed += digit_failed + punct_failed + alnum_failed + blank_failed + graph_failed + lower_failed + space_failed;

  // Performance test
  std::cout << "\n=== Performance Test ===\n";
  const int iterations = 10000000;

  // Test ASCII performance (should hit fast path)
  auto start = std::chrono::high_resolution_clock::now();
  volatile int sum = 0;
  for (int i = 0; i < iterations; i++) {
    sum += my_wctype::iswalpha('A' + (i % 26));
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto ascii_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "ASCII test (" << iterations << " calls): " << ascii_time << "ms\n";

  // Test Unicode performance (table lookup)
  start = std::chrono::high_resolution_clock::now();
  sum = 0;
  for (int i = 0; i < iterations; i++) {
    sum += my_wctype::iswalpha(0x0400 + (i % 256)); // Cyrillic range
  }
  end = std::chrono::high_resolution_clock::now();
  auto unicode_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "Unicode test (" << iterations << " calls): " << unicode_time << "ms\n";


  // Test std Unicode performance
  start = std::chrono::high_resolution_clock::now();
  sum = 0;
  for (int i = 0; i < iterations; i++) {
    sum += iswalpha(0x0400 + (i % 256)); // Cyrillic range
  }
  end = std::chrono::high_resolution_clock::now();
  auto std_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "Std unicode test (" << iterations << " calls): " << std_time << "ms\n";


  // Test CJK performance (should hit fast path)
  start = std::chrono::high_resolution_clock::now();
  sum = 0;
  for (int i = 0; i < iterations; i++) {
    sum += my_wctype::iswalpha(0x4E00 + (i % 100));
  }
  end = std::chrono::high_resolution_clock::now();
  auto cjk_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "CJK test (" << iterations << " calls): " << cjk_time << "ms\n";

  // Suppress unused variable warning
  (void) sum;

  return failed == 0 ? 0 : 1;
}
