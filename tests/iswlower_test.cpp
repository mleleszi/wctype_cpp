#include "wctype_test_base.h"
#include "mywctype.h"
#include <cwchar>

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
