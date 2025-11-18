#include "wctype_test_base.h"
#include "mywctype.h"

class WctypeMapTest : public WctypeTest {
};

TEST_F(WctypeMapTest, ValidPropertyNames) {
  EXPECT_EQ(my_wctype::wctype("alnum"), my_wctype::WCTYPE_ALNUM);
  EXPECT_EQ(my_wctype::wctype("alpha"), my_wctype::WCTYPE_ALPHA);
  EXPECT_EQ(my_wctype::wctype("blank"), my_wctype::WCTYPE_BLANK);
  EXPECT_EQ(my_wctype::wctype("cntrl"), my_wctype::WCTYPE_CNTRL);
  EXPECT_EQ(my_wctype::wctype("digit"), my_wctype::WCTYPE_DIGIT);
  EXPECT_EQ(my_wctype::wctype("graph"), my_wctype::WCTYPE_GRAPH);
  EXPECT_EQ(my_wctype::wctype("lower"), my_wctype::WCTYPE_LOWER);
  EXPECT_EQ(my_wctype::wctype("print"), my_wctype::WCTYPE_PRINT);
  EXPECT_EQ(my_wctype::wctype("punct"), my_wctype::WCTYPE_PUNCT);
  EXPECT_EQ(my_wctype::wctype("space"), my_wctype::WCTYPE_SPACE);
  EXPECT_EQ(my_wctype::wctype("upper"), my_wctype::WCTYPE_UPPER);
  EXPECT_EQ(my_wctype::wctype("xdigit"), my_wctype::WCTYPE_XDIGIT);
}

TEST_F(WctypeMapTest, InvalidPropertyNames) {
  EXPECT_EQ(my_wctype::wctype(""), 0);
  EXPECT_EQ(my_wctype::wctype("invalid"), 0);
  EXPECT_EQ(my_wctype::wctype("ALPHA"), 0);
  EXPECT_EQ(my_wctype::wctype("Alpha"), 0);
  EXPECT_EQ(my_wctype::wctype("alphanumeric"), 0);
  EXPECT_EQ(my_wctype::wctype("digits"), 0);
  EXPECT_EQ(my_wctype::wctype(" alpha"), 0);
  EXPECT_EQ(my_wctype::wctype("alpha "), 0);
}
