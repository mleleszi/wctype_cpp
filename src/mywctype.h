#ifndef MYWCTYPE_H
#define MYWCTYPE_H

#include "wctype_table.h"
#include <string_view>

namespace my_wctype {

namespace {
// constexpr wctype_t wctype_constexpr(std::string_view property) {
//   if (property == "alnum")
//     return PROP_ALPHA | PROP_DIGIT;
//   if (property == "alpha")
//     return PROP_ALPHA;
//   if (property == "blank")
//     return PROP_BLANK;
//   if (property == "cntrl")
//     return PROP_CNTRL;
//   if (property == "digit")
//     return PROP_DIGIT;
//   if (property == "graph")
//     return PROP_GRAPH;
//   if (property == "lower")
//     return PROP_LOWER;
//   if (property == "print")
//     return PROP_PRINT;
//   if (property == "punct")
//     return PROP_PUNCT;
//   if (property == "space")
//     return PROP_SPACE;
//   if (property == "upper")
//     return PROP_UPPER;
//   if (property == "xdigit")
//     return PROP_XDIGIT;
//   return 0;
// }
} // namespace


typedef uint16_t wctype_t;

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

// TODO: add tests
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

// inline wctype_t wctype(const char *property) {
//   return wctype_constexpr(property);
// }

// TODO: add tests
inline int iswctype(wchar_t wc, wctype_t desc) {
  if (desc == 0) {
    return 0;
  }

  uint8_t props = lookup_properties(wc);

  return (props & desc) != 0;
}

// TODO: iswprint
// TODO: iswxdigit

} // namespace my_wctype

#endif // MYWCTYPE_H
