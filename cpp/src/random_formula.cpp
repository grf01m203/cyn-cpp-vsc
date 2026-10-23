#include "cyn/random_formula.h"

#include <cmath>
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace cyn {

namespace {

constexpr int kMinNumber = 1;
constexpr int kMaxNumber = 20;
constexpr int kMaxDepth = 4;

std::string trim_number(double value) {
    std::ostringstream stream;
    stream.precision(6);
    stream << std::fixed << value;
    std::string text = stream.str();
    while (!text.empty() && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }
    return text;
}

class Parser {
public:
    explicit Parser(const std::string& text) : text_(text), position_(0) {}

    double parse() {
        double value = parse_expression();
        skip_whitespace();
        if (position_ != text_.size()) {
            throw std::runtime_error("表达式末尾存在多余字符");
        }
        return value;
    }

private:
    double parse_expression() {
        double value = parse_term();
        while (true) {
            skip_whitespace();
            if (consume('+')) {
                value += parse_term();
            } else if (consume('-')) {
                value -= parse_term();
            } else {
                return value;
            }
        }
    }

    double parse_term() {
        double value = parse_factor();
        while (true) {
            skip_whitespace();
            if (consume('*')) {
                value *= parse_factor();
            } else if (consume('/')) {
                double divisor = parse_factor();
                if (divisor == 0.0) {
                    throw std::runtime_error("除数不能为 0");
                }
                value /= divisor;
            } else {
                return value;
            }
        }
    }

    double parse_factor() {
        skip_whitespace();
        if (consume('+')) {
            return parse_factor();
        }
        if (consume('-')) {
            return -parse_factor();
        }
        if (consume('(')) {
            double value = parse_expression();
            skip_whitespace();
            if (!consume(')')) {
                throw std::runtime_error("缺少右括号");
            }
            return value;
        }
        return parse_number();
    }

    double parse_number() {
        skip_whitespace();
        const std::size_t start = position_;
        while (position_ < text_.size()
               && (std::isdigit(static_cast<unsigned char>(text_[position_])) != 0 || text_[position_] == '.')) {
            ++position_;
        }
        if (position_ == start) {
            throw std::runtime_error("无法识别的表达式片段");
        }
        try {
            return std::stod(text_.substr(start, position_ - start));
        } catch (const std::exception&) {
            throw std::runtime_error("数字格式错误");
        }
    }

    bool consume(char expected) {
        if (position_ < text_.size() && text_[position_] == expected) {
            ++position_;
            return true;
        }
        return false;
    }

    void skip_whitespace() {
        while (position_ < text_.size()
               && std::isspace(static_cast<unsigned char>(text_[position_])) != 0) {
            ++position_;
        }
    }

    const std::string& text_;
    std::size_t position_;
};

std::string strip_outer_parentheses(std::string expression) {
    if (expression.size() >= 2 && expression.front() == '(' && expression.back() == ')') {
        return expression.substr(1, expression.size() - 2);
    }
    return expression;
}

}  // namespace

RandomFormulaGenerator::RandomFormulaGenerator(std::uint32_t seed) : engine_(seed) {}

Formula RandomFormulaGenerator::next(int depth) {
    const int clamped = depth < 1 ? 1 : (depth > kMaxDepth ? kMaxDepth : depth);
    auto built = build(clamped);
    Formula formula;
    formula.expression = strip_outer_parentheses(built.first);
    formula.expected = built.second;
    return formula;
}

std::vector<Formula> RandomFormulaGenerator::generate(int count, int depth) {
    if (count < 0) {
        throw std::invalid_argument("count 不能为负数");
    }
    std::vector<Formula> formulas;
    formulas.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        formulas.push_back(next(depth));
    }
    return formulas;
}

std::pair<std::string, double> RandomFormulaGenerator::build(int depth) {
    std::uniform_int_distribution<int> number_distribution(kMinNumber, kMaxNumber);
    if (depth <= 1) {
        const int value = number_distribution(engine_);
        return {std::to_string(value), static_cast<double>(value)};
    }

    auto left = build(depth - 1);
    std::uniform_int_distribution<int> operator_distribution(0, 3);
    const int operator_index = operator_distribution(engine_);
    const char symbols[] = {'+', '-', '*', '/'};

    std::string expression;
    double value = 0.0;
    if (symbols[operator_index] == '/') {
        std::uniform_int_distribution<int> divisor_distribution(1, 9);
        const int divisor = divisor_distribution(engine_);
        expression = left.first + " / " + std::to_string(divisor);
        value = left.second / static_cast<double>(divisor);
    } else {
        auto right = build(depth - 1);
        const char symbol = symbols[operator_index];
        expression = left.first + " " + symbol + " " + right.first;
        if (symbol == '+') {
            value = left.second + right.second;
        } else if (symbol == '-') {
            value = left.second - right.second;
        } else {
            value = left.second * right.second;
        }
    }
    return {"(" + expression + ")", value};
}

double evaluate(const std::string& expression) {
    Parser parser(expression);
    return parser.parse();
}

std::string format(const Formula& formula) {
    return formula.expression + " = " + trim_number(formula.expected);
}

}  // namespace cyn
