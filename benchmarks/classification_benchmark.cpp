#include <benchmark/benchmark.h>
#include "mywctype.h"
#include <cwctype>
#include <vector>
#include <random>
#include <locale>

static void SetupBenchmarks() {
  std::locale::global(std::locale("C.UTF-8"));
}

// ============================================================================
// iswdigit benchmarks
// ============================================================================

static void BM_Iswdigit_My_ASCII_Digit(benchmark::State &state) {
  wint_t c = L'5';
  for (auto _ : state) {
    int result = my_wctype::iswdigit(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswdigit_My_ASCII_Digit);

static void BM_Iswdigit_Std_ASCII_Digit(benchmark::State &state) {
  wint_t c = L'5';
  for (auto _ : state) {
    int result = std::iswdigit(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswdigit_Std_ASCII_Digit);

static void BM_Iswdigit_My_ASCII_Letter(benchmark::State &state) {
  wint_t c = L'a';
  for (auto _ : state) {
    int result = my_wctype::iswdigit(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswdigit_My_ASCII_Letter);

static void BM_Iswdigit_Std_ASCII_Letter(benchmark::State &state) {
  wint_t c = L'a';
  for (auto _ : state) {
    int result = std::iswdigit(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswdigit_Std_ASCII_Letter);

static void BM_Iswdigit_My_Unicode(benchmark::State &state) {
  wint_t c = 0x00E0; // à
  for (auto _ : state) {
    int result = my_wctype::iswdigit(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswdigit_My_Unicode);

static void BM_Iswdigit_Std_Unicode(benchmark::State &state) {
  wint_t c = 0x00E0; // à
  for (auto _ : state) {
    int result = std::iswdigit(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswdigit_Std_Unicode);

static void BM_Iswdigit_My_Bulk_Mixed(benchmark::State &state) {
  std::vector<wint_t> text;
  const char *sample = "abc123xyz789!@#";
  for (const char *p = sample; *p; ++p) {
    text.push_back(static_cast<wint_t>(*p));
  }

  for (auto _ : state) {
    for (wint_t c : text) {
      int result = my_wctype::iswdigit(c);
      benchmark::DoNotOptimize(result);
    }
  }
  state.SetItemsProcessed(state.iterations() * text.size());
}
BENCHMARK(BM_Iswdigit_My_Bulk_Mixed);

static void BM_Iswdigit_Std_Bulk_Mixed(benchmark::State &state) {
  std::vector<wint_t> text;
  const char *sample = "abc123xyz789!@#";
  for (const char *p = sample; *p; ++p) {
    text.push_back(static_cast<wint_t>(*p));
  }

  for (auto _ : state) {
    for (wint_t c : text) {
      int result = std::iswdigit(c);
      benchmark::DoNotOptimize(result);
    }
  }
  state.SetItemsProcessed(state.iterations() * text.size());
}
BENCHMARK(BM_Iswdigit_Std_Bulk_Mixed);

// ============================================================================
// iswalpha benchmarks
// ============================================================================

static void BM_Iswalpha_My_ASCII_Letter(benchmark::State &state) {
  wint_t c = L'a';
  for (auto _ : state) {
    int result = my_wctype::iswalpha(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswalpha_My_ASCII_Letter);

static void BM_Iswalpha_Std_ASCII_Letter(benchmark::State &state) {
  wint_t c = L'a';
  for (auto _ : state) {
    int result = std::iswalpha(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswalpha_Std_ASCII_Letter);

static void BM_Iswalpha_My_ASCII_Digit(benchmark::State &state) {
  wint_t c = L'5';
  for (auto _ : state) {
    int result = my_wctype::iswalpha(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswalpha_My_ASCII_Digit);

static void BM_Iswalpha_Std_ASCII_Digit(benchmark::State &state) {
  wint_t c = L'5';
  for (auto _ : state) {
    int result = std::iswalpha(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswalpha_Std_ASCII_Digit);

static void BM_Iswalpha_My_Latin_Extended(benchmark::State &state) {
  wint_t c = 0x00E0; // à
  for (auto _ : state) {
    int result = my_wctype::iswalpha(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswalpha_My_Latin_Extended);

static void BM_Iswalpha_Std_Latin_Extended(benchmark::State &state) {
  wint_t c = 0x00E0; // à
  for (auto _ : state) {
    int result = std::iswalpha(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswalpha_Std_Latin_Extended);

static void BM_Iswalpha_My_Greek(benchmark::State &state) {
  wint_t c = 0x03B1; // α
  for (auto _ : state) {
    int result = my_wctype::iswalpha(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswalpha_My_Greek);

static void BM_Iswalpha_Std_Greek(benchmark::State &state) {
  wint_t c = 0x03B1; // α
  for (auto _ : state) {
    int result = std::iswalpha(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswalpha_Std_Greek);

static void BM_Iswalpha_My_CJK(benchmark::State &state) {
  wint_t c = 0x4E00; // CJK
  for (auto _ : state) {
    int result = my_wctype::iswalpha(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswalpha_My_CJK);

static void BM_Iswalpha_Std_CJK(benchmark::State &state) {
  wint_t c = 0x4E00; // CJK
  for (auto _ : state) {
    int result = std::iswalpha(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswalpha_Std_CJK);

static void BM_Iswalpha_My_Bulk_Mixed(benchmark::State &state) {
  std::vector<wint_t> text;
  const char *sample = "The Quick Brown Fox 123!";
  for (const char *p = sample; *p; ++p) {
    text.push_back(static_cast<wint_t>(*p));
  }

  for (auto _ : state) {
    for (wint_t c : text) {
      int result = my_wctype::iswalpha(c);
      benchmark::DoNotOptimize(result);
    }
  }
  state.SetItemsProcessed(state.iterations() * text.size());
}
BENCHMARK(BM_Iswalpha_My_Bulk_Mixed);

static void BM_Iswalpha_Std_Bulk_Mixed(benchmark::State &state) {
  std::vector<wint_t> text;
  const char *sample = "The Quick Brown Fox 123!";
  for (const char *p = sample; *p; ++p) {
    text.push_back(static_cast<wint_t>(*p));
  }

  for (auto _ : state) {
    for (wint_t c : text) {
      int result = std::iswalpha(c);
      benchmark::DoNotOptimize(result);
    }
  }
  state.SetItemsProcessed(state.iterations() * text.size());
}
BENCHMARK(BM_Iswalpha_Std_Bulk_Mixed);

// ============================================================================
// iswgraph benchmarks
// ============================================================================

static void BM_Iswgraph_My_ASCII_Letter(benchmark::State &state) {
  wint_t c = L'a';
  for (auto _ : state) {
    int result = my_wctype::iswgraph(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswgraph_My_ASCII_Letter);

static void BM_Iswgraph_Std_ASCII_Letter(benchmark::State &state) {
  wint_t c = L'a';
  for (auto _ : state) {
    int result = std::iswgraph(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswgraph_Std_ASCII_Letter);

static void BM_Iswgraph_My_ASCII_Digit(benchmark::State &state) {
  wint_t c = L'5';
  for (auto _ : state) {
    int result = my_wctype::iswgraph(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswgraph_My_ASCII_Digit);

static void BM_Iswgraph_Std_ASCII_Digit(benchmark::State &state) {
  wint_t c = L'5';
  for (auto _ : state) {
    int result = std::iswgraph(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswgraph_Std_ASCII_Digit);

static void BM_Iswgraph_My_ASCII_Space(benchmark::State &state) {
  wint_t c = L' ';
  for (auto _ : state) {
    int result = my_wctype::iswgraph(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswgraph_My_ASCII_Space);

static void BM_Iswgraph_Std_ASCII_Space(benchmark::State &state) {
  wint_t c = L' ';
  for (auto _ : state) {
    int result = std::iswgraph(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswgraph_Std_ASCII_Space);

static void BM_Iswgraph_My_Latin_Extended(benchmark::State &state) {
  wint_t c = 0x00E0; // à
  for (auto _ : state) {
    int result = my_wctype::iswgraph(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswgraph_My_Latin_Extended);

static void BM_Iswgraph_Std_Latin_Extended(benchmark::State &state) {
  wint_t c = 0x00E0; // à
  for (auto _ : state) {
    int result = std::iswgraph(c);
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Iswgraph_Std_Latin_Extended);

static void BM_Iswgraph_My_Bulk_Mixed(benchmark::State &state) {
  std::vector<wint_t> text;
  const char *sample = "Hello World 123!  \t\n";
  for (const char *p = sample; *p; ++p) {
    text.push_back(static_cast<wint_t>(*p));
  }

  for (auto _ : state) {
    for (wint_t c : text) {
      int result = my_wctype::iswgraph(c);
      benchmark::DoNotOptimize(result);
    }
  }
  state.SetItemsProcessed(state.iterations() * text.size());
}
BENCHMARK(BM_Iswgraph_My_Bulk_Mixed);

static void BM_Iswgraph_Std_Bulk_Mixed(benchmark::State &state) {
  std::vector<wint_t> text;
  const char *sample = "Hello World 123!  \t\n";
  for (const char *p = sample; *p; ++p) {
    text.push_back(static_cast<wint_t>(*p));
  }

  for (auto _ : state) {
    for (wint_t c : text) {
      int result = std::iswgraph(c);
      benchmark::DoNotOptimize(result);
    }
  }
  state.SetItemsProcessed(state.iterations() * text.size());
}
BENCHMARK(BM_Iswgraph_Std_Bulk_Mixed);

int main(int argc, char **argv) {
  SetupBenchmarks();
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();
  return 0;
}
