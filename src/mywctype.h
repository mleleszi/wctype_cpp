#ifndef MYWCTYPE_H
#define MYWCTYPE_H

#include "wctype_table.h"

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

#endif // MYWCTYPE_H
