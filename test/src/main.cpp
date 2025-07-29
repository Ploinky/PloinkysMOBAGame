#include <iostream>
#include "test-framework.h"

int main() {
    int passed = 0;
    int failed = 0;

    for (auto& test : GetAllTests()) {
        try {
            test.func();
            std::cout << "[PASS] " << test.name << "\n";
            passed++;
        } catch (...) {
            std::cout << "[FAIL] " << test.name << "\n";
            failed++;
        }
    }

    std::cout << "\nSummary: " << passed << " passed, " << failed << " failed.\n";

    return failed == 0 ? 0 : 1;
}