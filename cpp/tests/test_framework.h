#pragma once

// 一个极简的 C++ 单元测试框架：不需要任何第三方依赖，便于离线运行。
// 用法：
//   TEST_CASE(名字) { CHECK_EQ(3, add(1, 2)); }

#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace cyn_test {

class AssertionFailure : public std::runtime_error {
public:
    explicit AssertionFailure(const std::string& message) : std::runtime_error(message) {}
};

struct TestCase {
    std::string name;
    std::function<void()> function;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> cases;
    return cases;
}

struct Registrar {
    Registrar(const std::string& name, std::function<void()> function) {
        registry().push_back(TestCase{name, std::move(function)});
    }
};

template <typename T>
std::string format_value(const T& value) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

inline std::string format_value(const std::string& value) {
    return "\"" + value + "\"";
}

inline std::string format_value(bool value) {
    return value ? "true" : "false";
}

template <typename A, typename B>
void check_equal(const A& expected, const B& actual, const char* expression,
                 const char* file, int line) {
    if (!(expected == actual)) {
        std::ostringstream message;
        message << file << ":" << line << " CHECK_EQ(" << expression << ") 失败：期望 "
                << format_value(expected) << "，实际 " << format_value(actual);
        throw AssertionFailure(message.str());
    }
}

inline void check_true(bool condition, const char* expression, const char* file, int line) {
    if (!condition) {
        std::ostringstream message;
        message << file << ":" << line << " CHECK_TRUE(" << expression << ") 失败：条件不成立";
        throw AssertionFailure(message.str());
    }
}

inline void check_false(bool condition, const char* expression, const char* file, int line) {
    if (condition) {
        std::ostringstream message;
        message << file << ":" << line << " CHECK_FALSE(" << expression << ") 失败：条件成立";
        throw AssertionFailure(message.str());
    }
}

template <typename Exception, typename Function>
void check_throws(Function function, const char* expression, const char* file, int line) {
    bool thrown = false;
    try {
        function();
    } catch (const Exception&) {
        thrown = true;
    } catch (...) {
        std::ostringstream message;
        message << file << ":" << line << " CHECK_THROWS(" << expression << ") 失败：抛出了其他异常";
        throw AssertionFailure(message.str());
    }
    if (!thrown) {
        std::ostringstream message;
        message << file << ":" << line << " CHECK_THROWS(" << expression << ") 失败：没有抛出异常";
        throw AssertionFailure(message.str());
    }
}

inline int run_all() {
    int passed = 0;
    int failed = 0;
    for (const TestCase& test : registry()) {
        try {
            test.function();
            ++passed;
            std::cout << "[ PASS ] " << test.name << "\n";
        } catch (const AssertionFailure& error) {
            ++failed;
            std::cout << "[ FAIL ] " << test.name << "\n         " << error.what() << "\n";
        } catch (const std::exception& error) {
            ++failed;
            std::cout << "[ FAIL ] " << test.name << "\n         未捕获异常：" << error.what() << "\n";
        }
    }
    std::cout << "\n测试结果：" << passed + failed << " 个用例，通过 " << passed
              << "，失败 " << failed << "\n";
    return failed == 0 ? 0 : 1;
}

}  // namespace cyn_test

#define CYN_TEST_CONCAT_INNER(a, b) a##b
#define CYN_TEST_CONCAT(a, b) CYN_TEST_CONCAT_INNER(a, b)

#define TEST_CASE(test_name)                                                   \
    static void test_name();                                                   \
    static const ::cyn_test::Registrar CYN_TEST_CONCAT(registrar_, test_name)( \
        #test_name, &test_name);                                               \
    static void test_name()

#define CHECK_EQ(expected, actual) \
    ::cyn_test::check_equal((expected), (actual), #expected " == " #actual, __FILE__, __LINE__)

#define CHECK_TRUE(expression) \
    ::cyn_test::check_true(static_cast<bool>(expression), #expression, __FILE__, __LINE__)

#define CHECK_FALSE(expression) \
    ::cyn_test::check_false(static_cast<bool>(expression), #expression, __FILE__, __LINE__)

#define CHECK_THROWS(exception_type, expression)                                  \
    ::cyn_test::check_throws<exception_type>([&]() { (void)(expression); }, #expression, \
                                            __FILE__, __LINE__)
