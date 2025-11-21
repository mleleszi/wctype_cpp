#ifndef WCTYPE_SRC_HT_H
#define WCTYPE_SRC_HT_H

#include <unordered_map>
#include <unordered_set>
#include <wctype.h>

static std::unordered_map<wint_t, wint_t> ht_upper_map = {
#include "case_mapping_upper.inc"
};

static std::unordered_map<wint_t, wint_t> ht_lower_map = {
#include "case_mapping_lower.inc"
};

// TODO(bassiounix): remove unnecessary passes
// static std::unordered_set<wint_t> lower_classes = {
//     0x03F1, 0x1FBE, 0x1F96, 0x00B5, 0x1C81, 0x03D0, 0x03F5, 0x1F82, 0x0131,
//     0x1FA6, 0x1FF3, 0x1F91, 0x03D6, 0x1FB3, 0x03F0, 0x1FA2, 0x1C83, 0x1F94,
//     0x1F90, 0x1F80, 0x1C85, 0x1F81, 0x1C86, 0x00DF, 0x1F95, 0x1C80, 0x1FA7,
//     0x1C82, 0x03C2, 0x1E9B, 0x03D1, 0x1FA1, 0x1FA0, 0x1C88, 0x1FA5, 0x1F83,
//     0x1F86, 0x1F97, 0x03D5, 0x1FA3, 0x1F93, 0x1FA4, 0x1F84, 0x017F, 0x1F92,
//     0x1FC3, 0x1C87, 0x1F85, 0x1C84, 0x1F87};

// static std::unordered_set<wint_t> upper_classes = {
//     0x1F9D, 0x1FAB, 0x1FAF, 0x1F8B, 0x2126, 0x1FCC, 0x1FAD, 0x1F9A, 0x1F9C,
//     0x1F8D, 0x1FBC, 0x1FA9, 0x212A, 0x1E9E, 0x1FAC, 0x212B, 0x1FAA, 0x1F9E,
//     0x1F8A, 0x1F8E, 0x1F8C, 0x03F4, 0x1FFC, 0x1F98, 0x1F88, 0x1FA8, 0x1F89,
//     0x1F9B, 0x1FAE, 0x1F9F, 0x1F99, 0x1F8F, 0x0130};

inline wint_t towlower_impl(wint_t wc) noexcept {
  // Handle special cases
  if (wc == WEOF) {
    return WEOF;
  }

  // Out of valid Unicode range
  if (wc < 0 || wc > 0x10FFFF || (wc >= 0xD800 && wc <= 0xDFFF)) {
    return wc;
  }

  // ASCII fast path
  if (wc >= 'A' && wc <= 'Z') {
    return wc + 32;
  }
  if (wc <= 0x7F) {
    return wc;  // ASCII non-letters
  }

  if (ht_lower_map.find(wc) == ht_lower_map.end()) {
    return wc;
  }

  return ht_lower_map.at(wc);
}

inline wint_t towupper_impl(wint_t wc) noexcept {
  // Handle special cases
  if (wc == WEOF) {
    return WEOF;
  }

  // Out of valid Unicode range
  if (wc < 0 || wc > 0x10FFFF || (wc >= 0xD800 && wc <= 0xDFFF)) {
    return wc;
  }

  // ASCII fast path
  if (wc >= 'a' && wc <= 'z') {
    return wc - 32;
  }
  if (wc <= 0x7F) {
    return wc;  // ASCII non-letters
  }

  if (ht_upper_map.find(wc) == ht_upper_map.end()) {
    return wc;
  }

  return ht_upper_map.at(wc);
}
#endif  // WCTYPE_SRC_HT_H
