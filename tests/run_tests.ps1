# 자동 테스트 실행기
#   powershell -ExecutionPolicy Bypass -File tests\run_tests.ps1 [-Config Debug|Release] [-Filter 이름패턴] [-Build]
# tests\scenarios\*.txt 를 하나씩 게임 실행 파일의 테스트 모드(--test)로 돌리고 결과를 모은다.
# 시나리오 문법과 판정 기준은 tests\README.md 참고.
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Config = 'Debug',
    [string]$Filter = '*',
    [switch]$Build,
    [int]$TimeoutSec = 600
)

$ErrorActionPreference = 'Stop'
$Root = Split-Path -Parent $PSScriptRoot
$Exe = if ($Config -eq 'Debug') { Join-Path $Root 'OutputFile\bin_d\Client.exe' } else { Join-Path $Root 'OutputFile\bin\Client.exe' }
$OutDir = Join-Path $Root "tests\out\$Config"

if ($Build) {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    $msbuild = & $vswhere -latest -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\amd64\MSBuild.exe' | Select-Object -First 1
    & $msbuild (Join-Path $Root 'WinAPI_43.sln') -m -nologo -v:minimal "-p:Configuration=$Config" -p:Platform=x64
    if ($LASTEXITCODE -ne 0) { Write-Host '빌드 실패' -ForegroundColor Red; exit 1 }
}

if (-not (Test-Path $Exe)) { Write-Host "실행 파일이 없습니다: $Exe (-Build 로 빌드)" -ForegroundColor Red; exit 1 }
New-Item -ItemType Directory -Force $OutDir | Out-Null

$scenarios = Get-ChildItem (Join-Path $PSScriptRoot 'scenarios') -Filter '*.txt' | Where-Object { $_.BaseName -like $Filter } | Sort-Object Name
$results = @()

foreach ($s in $scenarios) {
    $name = $s.BaseName
    foreach ($ext in @('.log', '.crt.txt')) { Remove-Item (Join-Path $OutDir "$name$ext") -ErrorAction SilentlyContinue }

    $sw = [Diagnostics.Stopwatch]::StartNew()
    $p = Start-Process -FilePath $Exe -ArgumentList @('--test', "`"$($s.FullName)`"", '--out', "`"$OutDir`"") -WorkingDirectory $Root -PassThru
    if (-not $p.WaitForExit($TimeoutSec * 1000)) {
        $p.Kill()
        $code = 'TIMEOUT'
    } else {
        $code = $p.ExitCode
    }
    $sw.Stop()

    $log = Join-Path $OutDir "$name.log"
    $crt = Join-Path $OutDir "$name.crt.txt"
    $resultLine = if (Test-Path $log) { Get-Content $log -Encoding UTF8 | Where-Object { $_ -like 'RESULT *' } | Select-Object -Last 1 } else { $null }
    $allowLeaks = $resultLine -match 'allowleaks=1'
    $crtText = if (Test-Path $crt) { Get-Content $crt -Raw } else { '' }
    $leak = $crtText -match 'Detected memory leaks'
    $hang = $crtText -match 'HANG'

    $notes = @()
    if ($code -ne 0) { $notes += "exit=$code" }
    if ($leak) { $notes += $(if ($allowLeaks) { '누수(허용)' } else { '누수' }) }
    if ($hang) { $notes += '종료 멈춤' }
    $failLines = if (Test-Path $log) { Get-Content $log -Encoding UTF8 | Where-Object { $_ -match '^X \d+ FAIL|^ABORT|^CRASH|^SCRIPT ERROR' } } else { @() }

    $pass = ($code -eq 0) -and (-not $hang) -and ((-not $leak) -or $allowLeaks)
    $results += [pscustomobject]@{ Scenario = $name; Result = $(if ($pass) { 'PASS' } else { 'FAIL' }); Seconds = [math]::Round($sw.Elapsed.TotalSeconds, 1); Notes = ($notes -join ', ') }

    $color = if ($pass) { 'Green' } else { 'Red' }
    Write-Host ("{0,-4} {1} ({2}s) {3}" -f $(if ($pass) { 'PASS' } else { 'FAIL' }), $name, [math]::Round($sw.Elapsed.TotalSeconds, 1), ($notes -join ', ')) -ForegroundColor $color
    foreach ($l in $failLines) { Write-Host "       $l" -ForegroundColor DarkYellow }
}

$failed = @($results | Where-Object { $_.Result -ne 'PASS' }).Count
Write-Host ''
Write-Host ("{0}개 중 {1}개 통과, {2}개 실패 ({3})" -f $results.Count, ($results.Count - $failed), $failed, $Config)
exit $(if ($failed -gt 0) { 1 } else { 0 })
