// C++ 单元测试：随机公式生成与求值，由 CTest 调度运行。
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include "cyn/random_formula.h"
#include "test_framework.h"

using cyn::evaluate;
using cyn::format;
using cyn::Formula;
using cyn::RandomFormulaGenerator;

namespace {

bool close_enough(double left, double right, double tolerance = 1e-9) {
    return std::fabs(left - right) < tolerance;
}

}  // namespace

TEST_CASE(generator_is_deterministic_with_same_seed) {
    RandomFormulaGenerator first(2026);
    RandomFormulaGenerator second(2026);
    const std::vector<Formula> left = first.generate(10, 3);
    const std::vector<Formula> right = second.generate(10, 3);
    CHECK_EQ(static_cast<std::size_t>(10), left.size());
    for (std::size_t i = 0; i < left.size(); ++i) {
        CHECK_EQ(left[i].expression, right[i].expression);
    }
}

TEST_CASE(evaluate_respects_precedence) {
    CHECK_TRUE(close_enough(evaluate("1 + 2 * 3"), 7.0));
}

TEST_CASE(evaluate_respects_parentheses) {
    CHECK_TRUE(close_enough(evaluate("(1 + 2) * 3"), 9.0));
}

TEST_CASE(evaluate_supports_division) {
    CHECK_TRUE(close_enough(evaluate("7 / 2"), 3.5));
}

TEST_CASE(evaluate_supports_unary_minus) {
    CHECK_TRUE(close_enough(evaluate("-3 + 5"), 2.0));
}

TEST_CASE(generated_formulas_match_evaluator) {
    RandomFormulaGenerator generator(42);
    for (const auto& formula : generator.generate(100, 3)) {
        CHECK_TRUE(close_enough(evaluate(formula.expression), formula.expected));
    }
}

TEST_CASE(generation_never_divides_by_zero) {
    RandomFormulaGenerator generator(7);
    for (const auto& formula : generator.generate(200, 4)) {
        evaluate(formula.expression);
    }
}

TEST_CASE(evaluate_rejects_invalid_expressions) {
    CHECK_THROWS(std::runtime_error, evaluate("1 +"));
    CHECK_THROWS(std::runtime_error, evaluate("2 ^ 3"));
    CHECK_THROWS(std::runtime_error, evaluate("x + 1"));
}

TEST_CASE(evaluate_rejects_division_by_zero) {
    CHECK_THROWS(std::runtime_error, evaluate("1 / 0"));
}

TEST_CASE(format_contains_expression_and_value) {
    const Formula formula{"1 + 2", 3.0};
    CHECK_EQ(std::string("1 + 2 = 3"), format(formula));
}

TEST_CASE(shallow_depth_has_no_parentheses) {
    RandomFormulaGenerator generator(11);
    const Formula formula = generator.next(1);
    CHECK_FALSE(formula.expression.find('(') != std::string::npos);
}

TEST_CASE(generate_checks_count) {
    RandomFormulaGenerator generator(5);
    CHECK_EQ(static_cast<std::size_t>(0), generator.generate(0).size());
    CHECK_THROWS(std::invalid_argument, generator.generate(-1));
}

int main() {
    return cyn_test::run_all();
}
