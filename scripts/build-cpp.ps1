# 使用 Visual Studio 2022 的 CMake（MSBuild 生成器）配置并编译 cpp/ 目录。
# 说明：本机环境下 Ninja 生成器在 MSVC ABI 检测阶段会卡住，
# 因此统一改用 Visual Studio 2022 生成器，配置和构建都更稳定。
$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$cppDir = Join-Path $root 'cpp'
$buildDir = Join-Path $cppDir 'build'

$vswhere = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $vswhere)) {
    throw '未找到 vswhere.exe，请确认已安装 Visual Studio 2022（含 C++ 桌面开发工作负载）。'
}

$vsPath = (& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath) | Select-Object -First 1
if (-not $vsPath) {
    $vsPath = (& $vswhere -latest -products * -property installationPath) | Select-Object -First 1
}
if (-not $vsPath) {
    throw '未找到 Visual Studio 安装路径。'
}

$cmake = Join-Path $vsPath 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
if (-not (Test-Path $cmake)) {
    $cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
    if (-not $cmakeCommand) {
        throw '未找到 cmake，请安装 CMake 或 Visual Studio 的 CMake 组件。'
    }
    $cmake = $cmakeCommand.Source
}

Write-Host '[cyn-cpp-vsc] 配置 CMake（Visual Studio 2022 生成器）'
& $cmake --fresh -S $cppDir -B $buildDir -G 'Visual Studio 17 2022' -A x64
if ($LASTEXITCODE -ne 0) {
    Write-Host '[cyn-cpp-vsc] CMake 配置失败' -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host '[cyn-cpp-vsc] 编译 C++ 代码（Debug）'
& $cmake --build $buildDir --config Debug
if ($LASTEXITCODE -ne 0) {
    Write-Host '[cyn-cpp-vsc] C++ 编译失败' -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host '[cyn-cpp-vsc] C++ 编译完成' -ForegroundColor Green
exit 0
