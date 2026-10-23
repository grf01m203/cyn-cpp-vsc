#include <iostream>

#include "cyn/random_formula.h"

int main() {
    std::cout << "=== cyn-cpp-vsc 随机公式示例（C++）===\n";
    cyn::RandomFormulaGenerator generator(2026);
    for (const auto& formula : generator.generate(8, 3)) {
        std::cout << cyn::format(formula) << "\n";
    }
    std::cout << "校验 evaluate(\"1 + 2 * 3\") = " << cyn::evaluate("1 + 2 * 3") << "\n";
    return 0;
}
