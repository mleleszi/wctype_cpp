#ifndef MYWCTYPE_H
#define MYWCTYPE_H

#include "wctype_table.h"
// #include "case_mapping.h"
// #include "case_mapping_staged.h"
#include "ht.h"

#include <string_view>

namespace my_wctype {

using mywctype_t = uint8_t;

inline constexpr mywctype_t WCTYPE_ALNUM = 1;
inline constexpr mywctype_t WCTYPE_ALPHA = 2;
inline constexpr mywctype_t WCTYPE_BLANK = 3;
inline constexpr mywctype_t WCTYPE_CNTRL = 4;
inline constexpr mywctype_t WCTYPE_DIGIT = 5;
inline constexpr mywctype_t WCTYPE_GRAPH = 6;
inline constexpr mywctype_t WCTYPE_LOWER = 7;
inline constexpr mywctype_t WCTYPE_PRINT = 8;
inline constexpr mywctype_t WCTYPE_PUNCT = 9;
inline constexpr mywctype_t WCTYPE_SPACE = 10;
inline constexpr mywctype_t WCTYPE_UPPER = 11;
inline constexpr mywctype_t WCTYPE_XDIGIT = 12;

inline int iswalpha(wchar_t wc) {
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

  return lookup_properties(wc) & PROP_ALPHA;
}

inline int iswdigit(wchar_t wc) {
  // Fast path: ASCII digits only in C.UTF-8
  if (wc >= '0' && wc <= '9') {
    return 1;
  }
  return 0;
}

inline int iswpunct(wchar_t wc) {
  // TODO: ASCII fast path
  return lookup_properties(wc) & PROP_PUNCT;
}

inline int iswalnum(wchar_t wc) {
  // Fast path: ASCII alphanumeric
  if (wc <= 0x7F) {
    return (wc >= 'A' && wc <= 'Z') ||
           (wc >= 'a' && wc <= 'z') ||
           (wc >= '0' && wc <= '9');
  }

  // Fast path: Common CJK ranges
  if ((wc >= 0x4E00 && wc <= 0x9FFF) ||
      (wc >= 0x3400 && wc <= 0x4DBF) ||
      (wc >= 0x20000 && wc <= 0x2A6DF)) {
    return 1;
  }

  // only need to check alpha, digit already checked
  return lookup_properties(wc) & PROP_ALPHA;
}

inline int iswblank(wchar_t wc) {
  // TODO: fast path
  return lookup_properties(wc) & PROP_BLANK;
}

inline int iswgraph(wchar_t wc) {
  if (wc == 0x00A0) {
    return 1;
  }
  auto props = lookup_properties(wc);
  return (props & PROP_PRINT) && !(props & PROP_SPACE);
}

inline int iswlower(wchar_t wc) {
  // Fast path: ASCII lowercase
  if (wc >= 'a' && wc <= 'z') {
    return 1;
  }

  return lookup_properties(wc) & PROP_LOWER;
}

inline int iswupper(wchar_t wc) {
  // Fast path: ASCII uppercase
  if (wc >= 'A' && wc <= 'Z') {
    return 1;
  }

  return lookup_properties(wc) & PROP_UPPER;
}

inline int iswspace(wchar_t wc) {
  // TODO: fast path

  return lookup_properties(wc) & PROP_SPACE;
}

inline int iswprint(wchar_t wc) {
  // Fast path: ASCII printable characters
  if (wc >= 0x20 && wc <= 0x7E) {
    return 1;
  }

  return lookup_properties(wc) & PROP_PRINT;
}

inline int iswcntrl(wchar_t wc) {
  // Fast path: ASCII and C1 control characters
  if (wc <= 0x1F || (wc >= 0x7F && wc <= 0x9F)) {
    return 1;
  }

  return lookup_properties(wc) & PROP_CNTRL;
}

inline int iswxdigit(wchar_t wc) {
  if ((wc >= '0' && wc <= '9') ||
      (wc >= 'A' && wc <= 'F') ||
      (wc >= 'a' && wc <= 'f')) {
    return 1;
  }
  return 0;
}

inline mywctype_t wctype(const char *property) {
  std::string_view prop(property);

  if (prop == "alnum")
    return WCTYPE_ALNUM;
  if (prop == "alpha")
    return WCTYPE_ALPHA;
  if (prop == "blank")
    return WCTYPE_BLANK;
  if (prop == "cntrl")
    return WCTYPE_CNTRL;
  if (prop == "digit")
    return WCTYPE_DIGIT;
  if (prop == "graph")
    return WCTYPE_GRAPH;
  if (prop == "lower")
    return WCTYPE_LOWER;
  if (prop == "print")
    return WCTYPE_PRINT;
  if (prop == "punct")
    return WCTYPE_PUNCT;
  if (prop == "space")
    return WCTYPE_SPACE;
  if (prop == "upper")
    return WCTYPE_UPPER;
  if (prop == "xdigit")
    return WCTYPE_XDIGIT;

  return 0;
}

inline int iswctype(wchar_t wc, mywctype_t desc) {
  switch (desc) {
  case WCTYPE_ALNUM:
    return iswalnum(wc);
  case WCTYPE_ALPHA:
    return iswalpha(wc);
  case WCTYPE_BLANK:
    return iswblank(wc);
  case WCTYPE_CNTRL:
    return iswcntrl(wc);
  case WCTYPE_DIGIT:
    return iswdigit(wc);
  case WCTYPE_GRAPH:
    return iswgraph(wc);
  case WCTYPE_LOWER:
    return iswlower(wc);
  case WCTYPE_PRINT:
    return iswprint(wc);
  case WCTYPE_PUNCT:
    return iswpunct(wc);
  case WCTYPE_SPACE:
    return iswspace(wc);
  case WCTYPE_UPPER:
    return iswupper(wc);
  case WCTYPE_XDIGIT:
    return iswxdigit(wc);
  default:
    return 0;
  }
}

inline wint_t towlower(wint_t wc) {
  // ASCII fast path
  if (wc >= L'A' && wc <= L'Z') {
    return wc + (L'a' - L'A');
  }
  // Other ASCII characters don't need conversion
  if (wc <= 0x7F) {
    return wc;
  }
  return towlower_impl(wc);
}

inline wint_t towupper(wint_t wc) {
  // ASCII fast path
  if (wc >= L'a' && wc <= L'z') {
    return wc - (L'a' - L'A');
  }
  // Other ASCII characters don't need conversion
  if (wc <= 0x7F) {
    return wc;
  }
  return towupper_impl(wc);
}

} // namespace my_wctype

#endif // MYWCTYPE_H
