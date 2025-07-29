#pragma once

#include <iostream>
#include <vector>
#include <functional>

struct TestCase {
    const char* name;
    std::function<void()> func;
};

inline std::vector<TestCase>& GetAllTests() {
    static std::vector<TestCase> tests;
    return tests;
}

struct TestRegistrar {
    TestRegistrar(const char* name, std::function<void()> func) {
        GetAllTests().push_back({ name, func });
    }
};

#define TEST_CASE(name) \
    static void name(); \
    static TestRegistrar reg_##name(#name, name); \
    static void name()

#define REQUIRE(cond) do { \
    if (!(cond)) { \
        std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__ << ": " \
                  << "REQUIRE failed: " << #cond << "\n"; \
        throw std::runtime_error("Test failed."); \
    } \
} while (0)

#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "[CHECK FAIL] " << __FILE__ << ":" << __LINE__ << ": " \
                  << "CHECK failed: " << #cond << "\n"; \
    } \
} while (0)
