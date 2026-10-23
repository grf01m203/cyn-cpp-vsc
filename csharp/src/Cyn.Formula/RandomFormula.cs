using System.Globalization;

namespace Cyn.Formula;

/// <summary>一条随机公式：表达式文本和期望值。</summary>
public sealed record Formula(string Expression, double Expected);

/// <summary>随机四则运算公式生成器，生成阶段保证除数不为 0。</summary>
public sealed class RandomFormulaGenerator
{
    private const int MinNumber = 1;
    private const int MaxNumber = 20;
    private const int MaxDepth = 4;
    private static readonly char[] OperatorSymbols = ['+', '-', '*', '/'];

    private readonly Random _random;

    public RandomFormulaGenerator(int seed) => _random = new Random(seed);

    public RandomFormulaGenerator() : this(Environment.TickCount)
    {
    }

    /// <summary>生成一条公式，depth 为嵌套深度（1-4）。</summary>
    public Formula Next(int depth = 2)
    {
        var clamped = Math.Clamp(depth, 1, MaxDepth);
        var (expression, value) = Build(clamped);
        return new Formula(StripOuterParentheses(expression), value);
    }

    /// <summary>批量生成公式。</summary>
    public IReadOnlyList<Formula> Generate(int count, int depth = 2)
    {
        if (count < 0)
        {
            throw new ArgumentOutOfRangeException(nameof(count), "count 不能为负数");
        }

        var formulas = new List<Formula>(count);
        for (var index = 0; index < count; index++)
        {
            formulas.Add(Next(depth));
        }

        return formulas;
    }

    private (string Expression, double Value) Build(int depth)
    {
        if (depth <= 1)
        {
            var leaf = _random.Next(MinNumber, MaxNumber + 1);
            return (leaf.ToString(CultureInfo.InvariantCulture), leaf);
        }

        var left = Build(depth - 1);
        var symbol = OperatorSymbols[_random.Next(OperatorSymbols.Length)];

        if (symbol == '/')
        {
            // 除数固定为非零整数，从生成阶段就避免除零
            var divisor = _random.Next(1, 10);
            return ($"({left.Expression} / {divisor})", left.Value / divisor);
        }

        var right = Build(depth - 1);
        var value = symbol switch
        {
            '+' => left.Value + right.Value,
            '-' => left.Value - right.Value,
            _ => left.Value * right.Value,
        };
        return ($"({left.Expression} {symbol} {right.Expression})", value);
    }

    private static string StripOuterParentheses(string expression) =>
        expression.Length > 1 && expression[0] == '(' && expression[^1] == ')'
            ? expression[1..^1]
            : expression;

    /// <summary>计算四则运算表达式，仅支持数字、括号与 + - * /。</summary>
    public static double Evaluate(string expression) => new Parser(expression).Parse();

    /// <summary>格式化为 "表达式 = 值"。</summary>
    public static string Format(Formula formula) =>
        $"{formula.Expression} = {formula.Expected.ToString("0.######", CultureInfo.InvariantCulture)}";

    private sealed class Parser
    {
        private readonly string _text;
        private int _position;

        public Parser(string text) => _text = text ?? throw new ArgumentNullException(nameof(text));

        public double Parse()
        {
            var value = ParseExpression();
            SkipWhitespace();
            if (_position != _text.Length)
            {
                throw new FormatException("表达式末尾存在多余字符");
            }

            return value;
        }

        private double ParseExpression()
        {
            var value = ParseTerm();
            while (true)
            {
                SkipWhitespace();
                if (Consume('+'))
                {
                    value += ParseTerm();
                }
                else if (Consume('-'))
                {
                    value -= ParseTerm();
                }
                else
                {
                    return value;
                }
            }
        }

        private double ParseTerm()
        {
            var value = ParseFactor();
            while (true)
            {
                SkipWhitespace();
                if (Consume('*'))
                {
                    value *= ParseFactor();
                }
                else if (Consume('/'))
                {
                    var divisor = ParseFactor();
                    if (divisor == 0)
                    {
                        throw new DivideByZeroException("除数不能为 0");
                    }

                    value /= divisor;
                }
                else
                {
                    return value;
                }
            }
        }

        private double ParseFactor()
        {
            SkipWhitespace();
            if (Consume('+'))
            {
                return ParseFactor();
            }

            if (Consume('-'))
            {
                return -ParseFactor();
            }

            if (Consume('('))
            {
                var value = ParseExpression();
                SkipWhitespace();
                if (!Consume(')'))
                {
                    throw new FormatException("缺少右括号");
                }

                return value;
            }

            return ParseNumber();
        }

        private double ParseNumber()
        {
            SkipWhitespace();
            var start = _position;
            while (_position < _text.Length &&
                   (char.IsAsciiDigit(_text[_position]) || _text[_position] == '.'))
            {
                _position++;
            }

            if (_position == start)
            {
                throw new FormatException("无法识别的表达式片段");
            }

            return double.Parse(_text[start.._position], CultureInfo.InvariantCulture);
        }

        private bool Consume(char expected)
        {
            if (_position < _text.Length && _text[_position] == expected)
            {
                _position++;
                return true;
            }

            return false;
        }

        private void SkipWhitespace()
        {
            while (_position < _text.Length && char.IsWhiteSpace(_text[_position]))
            {
                _position++;
            }
        }
    }
}
