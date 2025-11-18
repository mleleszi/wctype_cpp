#include "wctype_test_base.h"
#include "mywctype.h"

class IswctypeTest : public WctypeTest {
};

TEST_F(IswctypeTest, Alnum) {
  auto desc = my_wctype::WCTYPE_ALNUM;

  EXPECT_NE(0, my_wctype::iswctype(L'a', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'Z', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'5', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L' ', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'!', desc));
}

TEST_F(IswctypeTest, Alpha) {
  auto desc = my_wctype::WCTYPE_ALPHA;

  EXPECT_NE(0, my_wctype::iswctype(L'a', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'Z', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'5', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L' ', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'!', desc));
}

TEST_F(IswctypeTest, Blank) {
  auto desc = my_wctype::WCTYPE_BLANK;

  EXPECT_NE(0, my_wctype::iswctype(L' ', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'\t', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'a', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'\n', desc));
}

TEST_F(IswctypeTest, Digit) {
  auto desc = my_wctype::WCTYPE_DIGIT;

  EXPECT_NE(0, my_wctype::iswctype(L'0', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'5', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'9', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'a', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'A', desc));
}

TEST_F(IswctypeTest, Graph) {
  auto desc = my_wctype::WCTYPE_GRAPH;

  EXPECT_NE(0, my_wctype::iswctype(L'a', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'!', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'5', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L' ', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'\n', desc));
}

TEST_F(IswctypeTest, Lower) {
  auto desc = my_wctype::WCTYPE_LOWER;

  EXPECT_NE(0, my_wctype::iswctype(L'a', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'z', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'A', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'Z', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'5', desc));
}

TEST_F(IswctypeTest, Punct) {
  auto desc = my_wctype::WCTYPE_PUNCT;

  EXPECT_NE(0, my_wctype::iswctype(L'!', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'.', desc));
  EXPECT_NE(0, my_wctype::iswctype(L',', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'a', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'5', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L' ', desc));
}

TEST_F(IswctypeTest, Space) {
  auto desc = my_wctype::WCTYPE_SPACE;

  EXPECT_NE(0, my_wctype::iswctype(L' ', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'\t', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'\n', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'\r', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'a', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'5', desc));
}

TEST_F(IswctypeTest, Upper) {
  auto desc = my_wctype::WCTYPE_UPPER;

  EXPECT_NE(0, my_wctype::iswctype(L'A', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'Z', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'a', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'z', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'5', desc));
}

TEST_F(IswctypeTest, Xdigit) {
  auto desc = my_wctype::WCTYPE_XDIGIT;

  EXPECT_NE(0, my_wctype::iswctype(L'0', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'9', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'a', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'f', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'A', desc));
  EXPECT_NE(0, my_wctype::iswctype(L'F', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'g', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'G', desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'z', desc));
}

TEST_F(IswctypeTest, InvalidDescriptor) {
  EXPECT_EQ(0, my_wctype::iswctype(L'a', 0));
  EXPECT_EQ(0, my_wctype::iswctype(L'5', 0));
  EXPECT_EQ(0, my_wctype::iswctype(L' ', 0));
}

TEST_F(IswctypeTest, WctypeIntegration) {
  auto alpha_desc = my_wctype::wctype("alpha");
  EXPECT_NE(0, my_wctype::iswctype(L'a', alpha_desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'5', alpha_desc));

  auto digit_desc = my_wctype::wctype("digit");
  EXPECT_NE(0, my_wctype::iswctype(L'5', digit_desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'a', digit_desc));

  auto xdigit_desc = my_wctype::wctype("xdigit");
  EXPECT_NE(0, my_wctype::iswctype(L'5', xdigit_desc));
  EXPECT_NE(0, my_wctype::iswctype(L'a', xdigit_desc));
  EXPECT_NE(0, my_wctype::iswctype(L'F', xdigit_desc));
  EXPECT_EQ(0, my_wctype::iswctype(L'g', xdigit_desc));

  auto invalid_desc = my_wctype::wctype("invalid");
  EXPECT_EQ(0, my_wctype::iswctype(L'a', invalid_desc));
}
