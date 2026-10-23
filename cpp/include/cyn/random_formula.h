#pragma once

#include <cstdint>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace cyn {

/// 一条随机公式：表达式文本和期望值。
struct Formula {
    std::string expression;
    double expected;
};

/// 生成随机四则运算公式（+ - * /），生成阶段保证除数不为 0。
class RandomFormulaGenerator {
public:
    explicit RandomFormulaGenerator(std::uint32_t seed = std::random_device{}());

    /// 生成一条公式，depth 为嵌套深度（1-4）。
    Formula next(int depth = 2);

    /// 批量生成公式。
    std::vector<Formula> generate(int count, int depth = 2);

private:
    std::pair<std::string, double> build(int depth);

    std::mt19937 engine_;
};

/// 计算四则运算表达式：仅支持数字、括号和 + - * /。
/// 非法表达式抛出 std::runtime_error，除零抛出 std::runtime_error。
double evaluate(const std::string& expression);

/// 格式化为 "表达式 = 值"。
std::string format(const Formula& formula);

}  // namespace cyn
