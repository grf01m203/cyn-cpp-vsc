# cyn-cpp-vsc：C++ / C# + VS Code 练习仓库

这是陈以诺（cyn）的 C++ / C# 练习仓库，使用 **Visual Studio Code** 打开和维护。

- 仓库名含义：`cyn` = 陈以诺，`cpp` = 本仓库同时包含 **C++** 与 **C#** 两种语言，`vsc` = Visual Studio Code。
- C++：C++17 + CMake + MSVC（Visual Studio 2022），测试用 CTest 调度。
- C#：.NET 10（`net10.0`），测试使用反射式轻量测试运行器（无需 NuGet 包即可离线运行）。
- 内容：**随机四则运算公式生成 / 求值工具**，两种语言各有一套实现与单元测试。

## 目录结构

```
cyn-cpp-vsc/
├─ .vscode/                       # VS Code 本地配置（已被 .gitignore 忽略）
│  ├─ settings.json               # CMake 源目录 / C++ 标准 / C# 解决方案
│  ├─ tasks.json                  # 构建、测试、全部测试任务
│  ├─ launch.json                 # C++ 与 C# 调试配置
│  └─ extensions.json             # 推荐扩展
├─ cpp/                           # C++ 项目
│  ├─ CMakeLists.txt
│  ├─ include/cyn/random_formula.h
│  ├─ src/random_formula.cpp      # 随机公式生成 + 递归下降求值
│  ├─ src/main.cpp                # 示例程序
│  └─ tests/                      # test_framework.h + test_main.cpp（12 个用例）
├─ csharp/                        # C# 项目
│  ├─ Cyn.CppVsc.slnx
│  ├─ src/Cyn.Formula/            # 随机公式生成 + 递归下降求值
│  └─ tests/Cyn.Tests/            # 测试运行器 + 12 个用例
├─ scripts/
│  ├─ build-cpp.ps1               # 用 VS 2022 的 CMake 配置并编译 C++
│  ├─ test-cpp.ps1                # 编译并运行 CTest
│  └─ test-csharp.ps1             # 运行 C# 单元测试
├─ run-tests.ps1                  # 一次性运行 C++ 和 C# 测试
└─ run-tests.bat                  # 双击即可运行全部测试
```

## 在 VS Code 中使用

1. 打开 VS Code，选择 **文件 → 打开文件夹**，选中本目录 `cyn-cpp-vsc`。
2. 推荐扩展：C/C++、C/C++ Extension Pack、CMake Tools、C# Dev Kit、GitLens（首次打开会提示）。
3. 构建 / 测试：
   - `Ctrl+Shift+B` 执行默认构建任务 **C++: 构建（Debug）**；
   - `Ctrl+Shift+P` → **Tasks: Run Test Task**，默认任务“运行全部测试（C++ + C#）”；
   - 也可以单独选择 “C++: 运行测试（CTest）” 或 “C#: 运行测试”。
4. 调试：**运行和调试** 面板选择 **C++: 运行示例程序（MSVC 调试）** 或 **C#: 运行测试（调试）**，按 F5。
5. Git：左侧 **源代码管理** 面板会自动识别 `.git`，可直接提交、查看历史。

## 命令行运行测试

```powershell
# 本目录下执行（或双击 run-tests.bat）
.\run-tests.bat

# 单独运行某一套
.\scripts\test-cpp.ps1
.\scripts\test-csharp.ps1
```

## 测试覆盖点（C++ 与 C# 各 12 个用例）

- 随机生成：相同种子结果可复现、生成数量校验、浅层表达式无括号
- 求值：运算优先级、括号、除法、一元负号
- 一致性：随机生成的 100 条公式，期望值与求值结果一致
- 安全性：生成阶段不会出现除零；1/0 抛异常；非法表达式（`1 +`、`2 ^ 3`、`x + 1`）被拒绝
- 格式化：输出 `表达式 = 值`
