# 依次运行 C++ 与 C# 两套测试，任意一套失败则整体失败。
$ErrorActionPreference = 'Stop'

$root = $PSScriptRoot
$suites = @(
    @{ Name = 'C++（CTest）'; Script = 'scripts\test-cpp.ps1' },
    @{ Name = 'C#（自定义测试运行器）'; Script = 'scripts\test-csharp.ps1' }
)

$failedSuites = @()
foreach ($suite in $suites) {
    Write-Host ''
    Write-Host ('===== {0} =====' -f $suite.Name) -ForegroundColor Cyan
    & powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $root $suite.Script)
    if ($LASTEXITCODE -ne 0) {
        $failedSuites += $suite.Name
        Write-Host ('[失败] {0}' -f $suite.Name) -ForegroundColor Red
    }
    else {
        Write-Host ('[通过] {0}' -f $suite.Name) -ForegroundColor Green
    }
}

Write-Host ''
if ($failedSuites.Count -gt 0) {
    Write-Host ('[cyn-cpp-vsc] 未通过：{0}' -f ($failedSuites -join '、')) -ForegroundColor Red
    exit 1
}
Write-Host '[cyn-cpp-vsc] C++ 与 C# 测试全部通过' -ForegroundColor Green
exit 0
