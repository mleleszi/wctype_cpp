#include "wctype_test_base.h"
#include "mywctype.h"

class XdigitTest : public WctypeTest,
                   public ::testing::WithParamInterface<TestCase> {
};

TEST_P(XdigitTest, IsXdigit) {
  auto test = GetParam();
  int my_result = my_wctype::iswxdigit(test.codepoint);
  int std_result = std::iswxdigit(static_cast<wint_t>(test.codepoint));

  EXPECT_EQ((my_result != 0), test.expected)
    << "Failed for U+" << std::hex << test.codepoint << " (" << test.name <<
 ")";
  EXPECT_EQ((my_result != 0), (std_result != 0))
    << "Mismatch with std for U+" << std::hex << test.codepoint << " (" << test.
name << ")";
}

INSTANTIATE_TEST_SUITE_P(XdigitTests, XdigitTest, ::testing::Values(
                           // ASCII decimal digits
                           TestCase{0x0030, "DIGIT ZERO", true},
                           TestCase{0x0031, "DIGIT ONE", true},
                           TestCase{0x0035, "DIGIT FIVE", true},
                           TestCase{0x0039, "DIGIT NINE", true},

                           // ASCII uppercase hex digits
                           TestCase{0x0041, "LATIN CAPITAL LETTER A", true},
                           TestCase{0x0042, "LATIN CAPITAL LETTER B", true},
                           TestCase{0x0043, "LATIN CAPITAL LETTER C", true},
                           TestCase{0x0044, "LATIN CAPITAL LETTER D", true},
                           TestCase{0x0045, "LATIN CAPITAL LETTER E", true},
                           TestCase{0x0046, "LATIN CAPITAL LETTER F", true},

                           // ASCII lowercase hex digits
                           TestCase{0x0061, "LATIN SMALL LETTER A", true},
                           TestCase{0x0062, "LATIN SMALL LETTER B", true},
                           TestCase{0x0063, "LATIN SMALL LETTER C", true},
                           TestCase{0x0064, "LATIN SMALL LETTER D", true},
                           TestCase{0x0065, "LATIN SMALL LETTER E", true},
                           TestCase{0x0066, "LATIN SMALL LETTER F", true},

                           // ASCII non-xdigit letters
                           TestCase{0x0047, "LATIN CAPITAL LETTER G", false},
                           TestCase{0x0067, "LATIN SMALL LETTER G", false},
                           TestCase{0x005A, "LATIN CAPITAL LETTER Z", false},
                           TestCase{0x007A, "LATIN SMALL LETTER Z", false},

                           // ASCII non-alphanumeric
                           TestCase{0x0020, "SPACE", false},
                           TestCase{0x0021, "EXCLAMATION MARK", false},
                           TestCase{0x002E, "FULL STOP", false},
                           TestCase{0x002B, "PLUS SIGN", false},
                           TestCase{0x007E, "TILDE", false},

                           // Unicode digits - not xdigits
                           TestCase{0x0660, "ARABIC-INDIC DIGIT ZERO", false},
                           TestCase{0x0966, "DEVANAGARI DIGIT ZERO", false},
                           TestCase{0xFF10, "FULLWIDTH DIGIT ZERO", false},
                           TestCase{0xFF21, "FULLWIDTH LATIN CAPITAL LETTER A",
                           false},
                           TestCase{0xFF41, "FULLWIDTH LATIN SMALL LETTER A",
                           false}
                         ));