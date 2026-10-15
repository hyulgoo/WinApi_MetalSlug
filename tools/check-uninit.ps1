# 생성자가 초기화하지 않은 멤버를 찾는다 (MS-R05)
#   powershell -ExecutionPolicy Bypass -File tools\check-uninit.ps1
# MSVC 정적 분석(C26495)은 생성자 안에서 멤버 함수를 부르면(init(), CreateCollider() 등) 그 함수가
# 초기화할 수도 있다고 보고 경고를 내지 않아, 이 코드에서는 대부분을 놓친다.
# 그래서 Visual Studio에 들어 있는 clang-tidy의 cppcoreguidelines-pro-type-member-init 검사를 쓴다.
# (Visual Studio 설치 관리자 > 개별 구성 요소 > "C++ Clang tools for Windows" 필요)
$ErrorActionPreference = 'Continue'   # clang-tidy가 stderr에 쓰는 요약을 오류로 보지 않게
$Root = Join-Path (Split-Path -Parent $PSScriptRoot) 'Client'
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsDir = & $vswhere -latest -property installationPath
$tidy = Join-Path $vsDir 'VC\Tools\Llvm\x64\bin\clang-tidy.exe'
if (-not (Test-Path $tidy)) { Write-Host "clang-tidy가 없습니다: $tidy"; exit 2 }

# vcvars64 환경(INCLUDE 경로)을 가져온다
$vcvars = Join-Path $vsDir 'VC\Auxiliary\Build\vcvars64.bat'
$env:PATH = "$(Split-Path $vswhere);$env:PATH"
cmd /c "`"$vcvars`" >nul && set" | ForEach-Object {
    if ($_ -match '^(INCLUDE|LIB|PATH)=(.*)$') { Set-Item -Path "env:$($Matches[1])" -Value $Matches[2] }
}

$args = @('-checks=-*,cppcoreguidelines-pro-type-member-init', '--quiet', '--',
          '-x', 'c++', '-std=c++17', '-fms-extensions', '-fms-compatibility', '--target=x86_64-pc-windows-msvc',
          "-I$Root", '-D_DEBUG', '-D_WINDOWS', '-DUNICODE', '-D_UNICODE', '-Wno-everything')

$warnings = @()
$files = @(Get-ChildItem (Join-Path $Root 'Game') -Recurse -Filter '*.cpp') + @(Get-Item (Join-Path $Root 'main.cpp'))
foreach ($f in $files) {
    $out = & $tidy $f.FullName @args 2>&1 | ForEach-Object { "$_" }
    $warnings += $out | Where-Object { $_ -match 'warning:' }
}
$warnings | Sort-Object -Unique | ForEach-Object { Write-Host $_ }
Write-Host ''
Write-Host ("미초기화 경고 {0}건 ({1}개 파일 검사)" -f @($warnings | Sort-Object -Unique).Count, $files.Count)
exit $(if ($warnings.Count -gt 0) { 1 } else { 0 })
