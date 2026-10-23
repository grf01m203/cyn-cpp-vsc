using System.Reflection;

namespace Cyn.Tests;

/// <summary>标记一个方法为测试用例。</summary>
[AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
public sealed class TestCaseAttribute : Attribute
{
}

/// <summary>断言失败时抛出的异常。</summary>
public sealed class AssertionException : Exception
{
    public AssertionException(string message) : base(message)
    {
    }
}

/// <summary>断言工具，类似 xUnit 的 Assert，但没有任何第三方依赖。</summary>
public static class Assert
{
    public static void Equal<T>(T expected, T actual, string? message = null)
    {
        if (!EqualityComparer<T>.Default.Equals(expected, actual))
        {
            throw new AssertionException(message ?? $"期望 {expected}，实际 {actual}");
        }
    }

    public static void NotEqual<T>(T unexpected, T actual, string? message = null)
    {
        if (EqualityComparer<T>.Default.Equals(unexpected, actual))
        {
            throw new AssertionException(message ?? $"期望不等于 {unexpected}，但实际相等");
        }
    }

    public static void True(bool condition, string? message = null)
    {
        if (!condition)
        {
            throw new AssertionException(message ?? "期望条件成立，但实际为 false");
        }
    }

    public static void False(bool condition, string? message = null)
    {
        if (condition)
        {
            throw new AssertionException(message ?? "期望条件不成立，但实际为 true");
        }
    }

    public static void Throws<TException>(Action action, string? message = null)
        where TException : Exception
    {
        try
        {
            action();
        }
        catch (TException)
        {
            return;
        }
        catch (Exception exception)
        {
            throw new AssertionException(
                message ?? $"期望抛出 {typeof(TException).Name}，实际抛出 {exception.GetType().Name}");
        }

        throw new AssertionException(message ?? $"期望抛出 {typeof(TException).Name}，但没有抛出异常");
    }
}

/// <summary>反射发现并运行所有 [TestCase]，打印结果并返回进程退出码。</summary>
public static class TestRunner
{
    public static int Run(Assembly assembly)
    {
        var testMethods = assembly.GetTypes()
            .SelectMany(type => type.GetMethods(BindingFlags.Public | BindingFlags.Instance | BindingFlags.Static)
                .Where(method => method.GetCustomAttribute<TestCaseAttribute>() is not null)
                .Select(method => (Type: type, Method: method)))
            .OrderBy(item => item.Type.Name)
            .ThenBy(item => item.Method.Name)
            .ToList();

        var passed = 0;
        var failed = 0;
        foreach (var (type, method) in testMethods)
        {
            var name = $"{type.Name}.{method.Name}";
            try
            {
                var instance = method.IsStatic ? null : Activator.CreateInstance(type);
                method.Invoke(instance, null);
                passed++;
                Console.WriteLine($"[ PASS ] {name}");
            }
            catch (TargetInvocationException exception) when (exception.InnerException is AssertionException assertion)
            {
                failed++;
                Console.WriteLine($"[ FAIL ] {name}");
                Console.WriteLine($"         {assertion.Message}");
            }
            catch (TargetInvocationException exception)
            {
                failed++;
                Console.WriteLine($"[ FAIL ] {name}");
                Console.WriteLine($"         未捕获异常：{exception.InnerException?.Message}");
            }
        }

        Console.WriteLine();
        Console.WriteLine($"C# 测试结果：{testMethods.Count} 个用例，通过 {passed}，失败 {failed}");
        return failed == 0 ? 0 : 1;
    }
}
