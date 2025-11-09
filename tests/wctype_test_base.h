#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <locale>

// Common test case structure
struct TestCase {
  uint32_t codepoint;
  const char *name;
  bool expected;
};

// Base test class with locale setup
class WctypeTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    std::locale::global(std::locale("C.UTF-8"));
  }
};
