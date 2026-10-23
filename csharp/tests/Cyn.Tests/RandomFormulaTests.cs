using System.Linq;
using Cyn.Formula;

namespace Cyn.Tests;

/// <summary>RandomFormulaGenerator 的单元测试。</summary>
public sealed class RandomFormulaTests
{
    [TestCase]
    public void GeneratorIsDeterministicWithSameSeed()
    {
        var first = new RandomFormulaGenerator(2026).Generate(10, 3);
        var second = new RandomFormulaGenerator(2026).Generate(10, 3);
        Assert.Equal(first.Count, second.Count);
        for (var index = 0; index < first.Count; index++)
        {
            Assert.Equal(first[index].Expression, second[index].Expression);
        }
    }

    [TestCase]
    public void EvaluateRespectsPrecedence() =>
        Assert.True(Math.Abs(RandomFormulaGenerator.Evaluate("1 + 2 * 3") - 7) < 1e-9);

    [TestCase]
    public void EvaluateRespectsParentheses() =>
        Assert.True(Math.Abs(RandomFormulaGenerator.Evaluate("(1 + 2) * 3") - 9) < 1e-9);

    [TestCase]
    public void EvaluateSupportsDivision() =>
        Assert.True(Math.Abs(RandomFormulaGenerator.Evaluate("7 / 2") - 3.5) < 1e-9);

    [TestCase]
    public void EvaluateSupportsUnaryMinus() =>
        Assert.True(Math.Abs(RandomFormulaGenerator.Evaluate("-3 + 5") - 2) < 1e-9);

    [TestCase]
    public void GeneratedFormulasMatchEvaluator()
    {
        foreach (var formula in new RandomFormulaGenerator(42).Generate(100, 3))
        {
            var actual = RandomFormulaGenerator.Evaluate(formula.Expression);
            Assert.True(Math.Abs(actual - formula.Expected) < 1e-9,
                $"公式 {formula.Expression} 期望 {formula.Expected}，实际 {actual}");
        }
    }

    [TestCase]
    public void GenerationNeverDividesByZero()
    {
        foreach (var formula in new RandomFormulaGenerator(7).Generate(200, 4))
        {
            RandomFormulaGenerator.Evaluate(formula.Expression);
        }
    }

    [TestCase]
    public void EvaluateRejectsInvalidExpressions()
    {
        Assert.Throws<FormatException>(() => RandomFormulaGenerator.Evaluate("1 +"));
        Assert.Throws<FormatException>(() => RandomFormulaGenerator.Evaluate("2 ^ 3"));
        Assert.Throws<FormatException>(() => RandomFormulaGenerator.Evaluate("x + 1"));
    }

    [TestCase]
    public void EvaluateRejectsDivisionByZero() =>
        Assert.Throws<DivideByZeroException>(() => RandomFormulaGenerator.Evaluate("1 / 0"));

    [TestCase]
    public void FormatContainsExpressionAndValue() =>
        Assert.Equal("1 + 2 = 3", RandomFormulaGenerator.Format(new Cyn.Formula.Formula("1 + 2", 3)));

    [TestCase]
    public void ShallowDepthHasNoParentheses()
    {
        var formula = new RandomFormulaGenerator(11).Next(1);
        Assert.False(formula.Expression.Contains('('));
    }

    [TestCase]
    public void GenerateChecksCount()
    {
        var generator = new RandomFormulaGenerator(5);
        Assert.Equal(0, generator.Generate(0).Count);
        Assert.Throws<ArgumentOutOfRangeException>(() => generator.Generate(-1));
    }
}
