using Cyn.Tests;

// 自定义测试运行器的入口：运行 Cyn.Tests 程序集里的全部 [TestCase]。
return TestRunner.Run(typeof(TestRunner).Assembly);
