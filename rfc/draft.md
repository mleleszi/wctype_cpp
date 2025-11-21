Credit note: Worked with @bassiounix and @michaelrj-google to come up with the possible solutions

## Motivation & Background

The wctype header is not currently implemented in LLVM libc.
This is a blocker for trying to build libc++ against LLVM libc.

What needs to be implemented:

- Wide character classification functions (iswalpha, iswdigit, etc..)
- Wide character conversion functions (towlower, towupper, etc..)
- Types and macros (wint_t, wctype_t, etc...)

The main crux of the implementation is coming up with an efficient way to represent
the required classification and conversion functions, with full C.UTF8 support (1,114,112 codepoints)
without bloating the binary size.

## Goals

A fully standard compliant C.UTF8 and C locale implementation. Support for other locales is not currently planned.
</br>
[ISO C standard](https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/wctype.h.html)

## Classification functions

For each code point, we can define a bitmask where each bit would represent a Unicode category,
like alpha, digit, upper, etc. There's 12 categories required for the wctype implementation, so this would fit in 2
bytes.
We could store bitmasks in some kind of lookup table. However, not all properties need a lookup table,
as some properties are composite ones (alnum, graph), and some properties can be hardcoded for C.UTF8 (for example, in
C.UTF8 only ASCII digits are considered digits),
so in practice we can get away with using a single byte flag.

### Proposed solution: Two stage lookup table

We can take advantage of the clustered nature of Unicode assignments, where contiguous ranges of
code points frequently share the same classification properties and don't store a mask for each property,
but for blocks of properties.

**Index array**: The 21 bit code point is split. The upper 13 bits would index into the index table, where each
entry would represent a block of 256 (the remaining 8 bits) consecutive characters.
The value store here is an index into the block table.

**Block array**: This array contains the actual property masks. Crucially, the identical blocks are deduplicated.
If multiple 256-character ranges in Unicode have the exact same properties, they'd all point
to the same entry in the block array.

- Properties to be stored in the lookup tables
    - upper, lower, alpha, space, print, blank, control, punct
- Properties that don't need a lookup table
    - alnum: composite, can be defined as alpha && digit
    - graph: composite, can be defined as print && !space
    - digit: in C.UTF8, only ASCII digits are considered digits, can be hardcoded
    - xdigit: only few possibilites, can be hardcoded

These arrays could be generated at build time and could be included in the binary itself.
According to my prototype implementation, this would increase the code size by **~46KB**.
This approach would provide constant time lookup for the classification functions.

We can also implement fast paths for ASCII cases for all properties, skipping the need for a lookup table in those
cases.

Benchmarking this against glibc is showing to this implementation be an order of magnitude faster.

### Alternatives

We could do a similar approach as glibc, and store the lookup tables on disk which would then be loaded
on locale change, and get memory mapped into memory to be used in the actual wctype function calls. This
would lead to a smaller binary size and could support locale switching at runtime.

However, it would have a lot of drawbacks. It'd introduce a runtime dependency on the locale files, which we'd have to
provide separately from the library itself, and there's also the overhead of the initial loading from disk.

I propose that we include the lookup tables in the library itself, and that this increase of the binary size by ~46KB is
acceptable, it's fine in server environments, and for size
constrained environments like embedded,
we could provide a flag for turning this feature off.

## Conversion functions

We have multiple options here, depending on if we want to prioritize speed vs size.

### Possible solution: sorted array and binary search lookup

There's two functions we need to implement here: towlower, and towupper.
We could store the actual conversions (codepoint pairs) in a sorted array, and do binary search on lookup.
We'd need two arrays, and array mapping from lower to upper and vice versa. There's roughly 2800 codepoints that have a
conversion,
and each mapping would be 8 bytes (wint_t -> wint_t), so this would end up being **~23KB**.

Lookup would be logarithmic, but it might be acceptable as are arrays would be of size ~1400
and we can also implement fast paths for ASCII cases for all properties, skipping the need for doing binary search.

I've benchmarked my prototype implementation against glibc, and this approach seems ~3 times slower in non ASCII cases
compared to glibc.

### Possible solution: two stage lookup table

We could use a similar two-level table approach as the classification functions.
The mappings would be stored in 256-entry blocks indexed by the upper 8 bits of the codepoint. Since case mappings are
sparse (only ~2,800 codepoints), most blocks are empty and can share a single zero-filled block.
This would provide constant-time lookup, but my prototype implementation shows this be around **~50KB**

Benchmarking this against glibc is showing to this implementation be an order of magnitude faster.

### Possible solution: hashmap

TODO(bassiounix)

## Build integration

I propose that the generation scripts, which can be written in any language (I'm planning on using C++ though), should
not be included in LLVM libc's build system.
The generation scripts will be checked into the repository, but they should be run manually on Unicode version upgrades,
and not on every build, and only the generated files themselves will be included in the build.

## Benchmarks

Benchmark results can be
seen [here](Benchmarking this against glibc is showing to this implementation be an order of magnitude faster.
). This might not be totally accurate and representative, only quick prototyping was done.

## Prototype code

[https://github.com/mleleszi/wctype_prototype](https://github.com/mleleszi/wctype_prototype)





