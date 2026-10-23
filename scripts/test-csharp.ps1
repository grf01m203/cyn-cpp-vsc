# 编译并运行 C# 单元测试（自定义轻量级测试运行器，离线可用）。
$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$project = Join-Path $root 'csharp\tests\Cyn.Tests\Cyn.Tests.csproj'

if (-not (Get-Command dotnet -ErrorAction SilentlyContinue)) {
    throw '未找到 dotnet，请安装 .NET SDK 10。'
}

Write-Host '[cyn-cpp-vsc] 运行 C# 单元测试'
& dotnet run --project $project
if ($LASTEXITCODE -ne 0) {
    Write-Host '[cyn-cpp-vsc] C# 测试失败' -ForegroundColor Red
    exit 1
}
Write-Host '[cyn-cpp-vsc] C# 测试通过' -ForegroundColor Green
exit 0
