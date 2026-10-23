# 编译并运行 C++ 单元测试（CTest）。
$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$buildDir = Join-Path $root 'cpp\build'
$buildScript = Join-Path $PSScriptRoot 'build-cpp.ps1'

& powershell -NoProfile -ExecutionPolicy Bypass -File $buildScript
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

# 定位 CTest（优先使用 Visual Studio 自带的 CMake 目录）
$vswhere = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'
$ctest = $null
if (Test-Path $vswhere) {
    $vsPath = (& $vswhere -latest -products * -property installationPath) | Select-Object -First 1
    $candidate = Join-Path $vsPath 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe'
    if (Test-Path $candidate) {
        $ctest = $candidate
    }
}
if (-not $ctest) {
    $ctestCommand = Get-Command ctest -ErrorAction SilentlyContinue
    if ($ctestCommand) {
        $ctest = $ctestCommand.Source
    }
}
if (-not $ctest) {
    throw '未找到 ctest。'
}

Write-Host '[cyn-cpp-vsc] 运行 C++ 单元测试（CTest）'
& $ctest --test-dir $buildDir -C Debug --output-on-failure
if ($LASTEXITCODE -ne 0) {
    Write-Host '[cyn-cpp-vsc] C++ 测试失败' -ForegroundColor Red
    exit 1
}
Write-Host '[cyn-cpp-vsc] C++ 测试通过' -ForegroundColor Green
exit 0
