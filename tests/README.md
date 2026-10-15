# 자동 테스트

게임 실행 파일을 **테스트 모드**로 실행해 시나리오를 재생하고 결과를 검사한다.
GDI 게임이라 단위 테스트 대신, 같은 입력이면 같은 결과가 나오는 재생 실행을 검증 수단으로 쓴다.

```powershell
# 전부 실행 (Debug 빌드)
powershell -ExecutionPolicy Bypass -File tests\run_tests.ps1
# 빌드부터, 이름으로 거르기
powershell -ExecutionPolicy Bypass -File tests\run_tests.ps1 -Build -Filter "stage1_*"
# 시나리오 하나만 직접
OutputFile\bin_d\Client.exe --test tests\scenarios\smoke_stage.txt --out tests\out\Debug
```

결과는 `tests\out\<구성>\` 에 남는다.

| 파일 | 내용 |
| --- | --- |
| `<이름>.log` | 프레임 상태(`F`), 오브젝트 생성·삭제와 레벨 진입(`E`), 몬스터(`M`), 기대값 판정(`X`), 오류(`ERROR`), 마지막 줄 `RESULT` |
| `<이름>.crt.txt` | CRT 보고. 누수(`Detected memory leaks`), assert, 힙 오류, 종료 멈춤(`HANG`) |
| `<이름>_<캡처>.bmp` | `capture` 명령으로 저장한 화면 |

## 테스트 모드가 하는 일

- 창을 띄우지 않는다. 키보드·마우스 대신 시나리오의 입력을 쓴다
- 프레임 시간(DT)을 고정하고(기본 1/60초) 난수 시드를 고정한다 → 같은 시나리오는 항상 같은 결과
- 메시지 상자 대신 로그에 남긴다(`ERROR`). 예/아니오 질문은 늘 예(`ASK` 줄). 대화상자는 `dialog` 명령의 답을 쓴다
- 처리되지 않은 예외는 호출 스택을 로그에 남기고 종료 코드 3, CRT assert·힙 오류는 4, 종료 멈춤은 5

종료 코드: 0 통과, 1 기대값 실패, 2 시나리오 문법 오류, 3 크래시, 4 CRT 오류, 5 종료 멈춤.
실행기는 종료 코드 0이고 누수 보고가 없어야(`allowleaks` 시나리오 제외) 통과로 본다.

## 시나리오 문법

한 줄에 명령 하나, `#` 뒤는 주석. 프레임 번호는 0부터 센다.
`<frame>` 자리에 `end`를 쓰면 마지막 프레임이 끝난 뒤를 뜻한다.
프레임 수 대신 `2.5s`처럼 초를 쓰면 고정 DT로 프레임 번호를 구한다(`dt`가 파일 어디에 있든 먼저 읽는다).
DT만 다른 시나리오(예: `s3_*_30` `_60` `_144`)가 같은 시각을 쓰게 할 때 쓴다.

| 명령 | 뜻 |
| --- | --- |
| `start <LEVEL>` | 시작 레벨. `START` `STAGE` `ANIMEDITOR` `GAMEOVER` |
| `frames <n>` | 실행할 프레임 수 (`6s`처럼 초도 된다) |
| `dt <초>` | 고정 프레임 시간 (기본 0.0166667) |
| `seed <n>` | 난수 시드 (기본 1) |
| `log <n>` | n 프레임마다 상태 한 줄 (0이면 끄기, 기본 1) |
| `logmon <n>` | n 프레임마다 몬스터 목록 |
| `nospawn` | 레벨 파일의 몬스터 배치를 쓰지 않는다 |
| `heapcheck` | Debug 힙 검사 강화(해제 지연 + 1024번 할당마다 검사). 느리다 |
| `debugonly` | Debug 빌드에서만 돌린다(디버그 키 등). Release에서는 바로 `RESULT SKIP`으로 끝나고 실행기는 건너뜀으로 센다 |
| `allowleaks` | 누수 보고가 있어도 실패로 보지 않는다 |
| `data <파일> <구역> <키> <값>` | 게임 수치 데이터 파일(`content\data\<파일>.ini`)의 값 하나를 실행 내내 바꿔 쓴다. 값에 공백이 있어도 된다(`data player life spawn_pos 300, 500`). 파일에 없는 키면 `ERROR` |
| `hold <from> <to> <KEY...>` | from~to 프레임 동안 키를 누르고 있다 |
| `tap <frame> <KEY...>` | 그 프레임에만 누른다 |
| `mouse <frame> <x> <y>` | 그 프레임부터 마우스 위치 |
| `dialog <답>` | 다음 대화상자의 답(쓰인 순서대로 하나씩). 파일 대화상자는 경로: `$out\a.anim`은 이번 실행의 출력 폴더, 상대 경로는 `OutputFile\bin\content\` 기준, 저장 대화상자에서 확장자가 없으면 기본 확장자를 붙인다. 애니메이션 에디터의 "격자로 프레임 추가"는 `왼쪽,위,너비,높이,개수,시간(1/100초)`. 남은 답이 없으면 취소 |
| `level <frame> <LEVEL>` | 레벨 전환 요청 |
| `spawn <frame> <TYPE> <x> <y>` | 화면 좌표에 몬스터 등장. `SOLDIER` `ZB_GIRL` `ZB_BOY` `ZB_MAN` `ZB_OLD` `ZB_RSH` `BOSS`(좌표는 몸 그림의 가운데) |
| `camera <frame> <x>` | 카메라 왼쪽을 배경 텍스처 x로 바로 옮긴다(높이는 카메라 경로). 플레이어는 화면 위치 그대로 따라오고, 그 x보다 앞에서 나올 몬스터 배치는 건너뛴다. 보스는 `camera 0.5s 5150`(보스가 나오는 자리) 뒤 `camera 0.55s 5182`(스테이지 끝) |
| `god <frame> <0\|1>` · `hmg <frame> <0\|1>` | 플레이어 무적·HMG |
| `teleport <frame> <x> <y>` | 플레이어를 화면 좌표로 옮긴다 |
| `play <frame> <클래스> <애니메이션>` | 그 클래스 첫 오브젝트가 애니메이션을 재생하고 바로 방향을 정한다(게임 코드처럼). 없는 애니메이션일 때를 본다 |
| `loadanim <frame> <클래스> <파일>` | 그 클래스 첫 오브젝트의 애니메이터가 `.anim` 파일을 불러온다 |
| `loadtex <frame> <키> <경로>` | 텍스처를 불러온다 |
| `kill <frame>` | 플레이어를 죽인다. 시간 초과와 같은 규칙이라 무적(`god 1`)이면 죽지 않는다 |
| `timer <frame> <n>` | 스테이지 남은 시간 표시값 |
| `spike <frame> <dt>` | 그 프레임만 DT를 바꾼다(창 끌기·브레이크포인트 같은 긴 프레임 흉내) |
| `watch <클래스>` | 그 클래스 오브젝트의 위치 최솟값·최댓값을 매 프레임 기록 (화면 좌표와 월드 좌표 둘 다) |
| `watchanim <클래스>` | 그 클래스 오브젝트의 애니메이션이 바뀔 때마다 기록하고 센다. 두 애니메이터의 애니메이션·프레임을 처음 보인 시각도 남긴다 |
| `trace <클래스>` | 그 클래스 오브젝트마다 매 프레임 위치와 두 애니메이터의 애니메이션·프레임 번호를 `T` 줄로 남긴다 |
| `capture <frame> <이름>` | 화면을 BMP로 저장 |
| `expect <frame> <대상> <연산자> <값>` | 기대값 검사. 연산자 `==` `!=` `<` `<=` `>` `>=` |

키 이름: `UP DOWN LEFT RIGHT SPACE ENTER ESC LALT LCTRL LSHIFT TAB LBTN RBTN Q W E R A S D F I K J L T Z X C V 0~8 F1~F6 DEL`

### expect 대상

| 대상 | 값 |
| --- | --- |
| `level` | 현재 레벨 이름 |
| `errors` | `ERROR`(리소스 로드·재생 실패, 데이터 파일의 없는 값·틀린 값 등) 횟수 |
| `created.<클래스>` · `deleted.<클래스>` | 이벤트로 생성·삭제된 수 (예: `created.CBomb`) |
| `alive.<클래스>` | 지금 레이어에 살아 있는 수 |
| `layer.<번호>` · `monsters` | 레이어(번호는 `LAYER` 열거값)·몬스터 레이어의 살아 있는 수 |
| `enter.<LEVEL>` | 레벨 진입 횟수 |
| `stat.gdi.enter.<LEVEL>.<n>` · `stat.mem.enter.<LEVEL>.<n>` | n번째 진입 때 GDI 개체 수·전용 메모리(바이트) |
| `gdi` · `mem` | 지금의 GDI 개체 수·전용 메모리 |
| `player.life` `player.bomb` `player.bullet` `player.hmg` `player.x` `player.y` | 플레이어 상태. 위치는 화면 좌표 |
| `player.anim` · `player.anim2` | 상체(전신)·하체 애니메이션 이름 |
| `cam.x` · `cam.y` | 카메라 왼쪽 위 (배경 텍스처 좌표) |
| `cam.fade` · `cam.offset.x` | 페이드 비율(0이면 그리지 않음), 셰이크 오프셋 |
| `overlaps` | 충돌 매니저가 기억하는 "지금 겹쳐 있는 충돌체 쌍" 수 |
| `count.anim.parse` · `anim.files` | `.anim` 파일을 디스크에서 읽어 해석한 횟수, 그중 서로 다른 파일 수 |
| `perf.tick` · `perf.render` | 처음 30프레임을 뺀 프레임 평균 시간(ms). PC·빌드에 따라 다르므로 같은 조건의 전후 비교에만 쓴다. `RESULT` 줄에도 남는다 |
| `stage.time` | 스테이지 남은 시간 표시값 |
| `first.<클래스>.<필드>` · `last.<클래스>.<필드>` | 살아 있는 첫·가장 최근 오브젝트의 필드. `x` `y` `id` `dead` `colw` `colh`(충돌체 크기) `hp` `state`(몬스터 FSM 상태: `IDLE` `TRACE` `TRACE_SLY` `ATTACK` `ATTACK_CQC` `DEAD` `APPEAR` `WAIT`) `anim` `anim2` `frm` `frm2`(애니메이션 프레임 번호) `wx` `wy`(월드 위치: 화면 픽셀 배율, 배경 텍스처 1px = 4) `collag`(충돌체가 오너 위치+오프셋에서 떨어진 거리). 없으면 문자열 `none` |
| `firstframe.created.<클래스>` · `firsttime.created.<클래스>` | 처음 생성된 프레임·시각(초). 없으면 -1. `deleted`는 처음 삭제된 때 |
| `firstframe.anim.<클래스>.<애니메이션>` · `firsttime.anim.<클래스>.<애니메이션>@<n>` | `watchanim`으로 본, 그 애니메이션(뒤에 `@n`을 붙이면 그 n번 프레임)을 처음 보인 프레임·시각. 없으면 -1 |
| `lifetime.<클래스>` | 그 클래스에서 처음 삭제된 오브젝트가 생성부터 삭제까지 산 시간(초). 없으면 -1 |
| `delfrm.<클래스>` | 그 클래스에서 처음 삭제된 오브젝트가 지워질 때 보이던 애니메이션 프레임 번호. 없으면 -1 |
| `min.<클래스>.x` · `max.<클래스>.y` 등 | `watch`로 기록한 위치 범위 (화면 좌표) |
| `maxcollag.<클래스>` | `watch`로 본 충돌체 지연 거리(`collag`)의 최댓값. 0이면 충돌 판정이 늘 그 프레임의 위치로 된다 |
| `wrange.<클래스>.x` · `wrange.<클래스>.y` | `watch`로 기록한 월드 위치의 폭(최댓값 - 최솟값). 그 클래스 오브젝트를 모두 합친 범위라 하나만 있을 때 쓴다. 가만히 선 오브젝트가 배경에 붙어 있으면 0 |
| `animcount.<클래스>.<애니메이션>` | `watchanim`으로 센, 그 애니메이션이 시작된 횟수 |
| `count.<이름>` | 게임 코드가 `CTestMgr::AddCount`로 센 값 |
| `editor.<필드>` | 애니메이션 에디터의 편집 중인 애니메이션(에디터가 아니면 대상 없음). `frames` `cur`(0부터) `dirty`(저장하지 않은 변경) `undo`(되돌리기 수) `paused` `magni` `sheetx` `sheety`(시트 화면 위치), 문자열 `sheet`(아틀라스 키) `name` `ref`(참고 애니메이션) `preview`(`NORMAL` `UPPER` `LOWER`), 고르고 있는 프레임의 `lt.x` `lt.y` `size.x` `size.y` `offset.x` `offset.y` `dur` |
| `animfile.<경로>.<필드>` | `.anim` 파일을 엔진 코드로 읽은 값. `frames`(못 읽으면 -1), 문자열 `atlas` `atlaspath` `name`(못 읽으면 `none`). 경로는 `dialog`와 같은 규칙 |
| `animdata.<경로>.frames` | 게임이 읽어 둔 애니메이션 데이터(없으면 지금 읽는다)의 프레임 수. 경로는 `animation` 폴더 기준(`loadanim`과 같다) |
| `difflines.<파일A>\|<파일B>` | 두 UTF-16 텍스트 파일에서 같은 줄 번호끼리 다른 줄 수. 하나라도 못 읽으면 -1. 경로는 `dialog`와 같은 규칙 |
| `frame` | 지금 프레임 |

값 자리에는 숫자, 문자열, 다른 대상, 또는 `대상+숫자`·`대상-숫자`를 쓸 수 있다.
예) `expect end stat.gdi.enter.START.10 <= stat.gdi.enter.START.2`,
`expect end stat.mem.enter.STAGE.10 <= stat.mem.enter.STAGE.2+4000000`

## 시나리오 이름

| 접두어 | 뜻 |
| --- | --- |
| `smoke_` | 테스트 모드 자체 확인 |
| `checklist_` | 수동 점검표(`docs/manual-checklist.md`)를 자동으로 도는 흐름 |
| `s1_` ~ `s9_` | 리팩토링 단계별 결함 재현. 수정 전에는 실패하고 수정 뒤 통과해야 한다. 파일 첫 줄에 항목 번호(MS-Rxx) |
| `bug_` | 리팩토링 뒤 사용자가 보고했거나 점검에서 찾은 결함의 재현. 수정 전에는 실패하고 수정 뒤 통과해야 한다 |
| `data_` | 게임 수치 데이터 파일을 읽는지, 틀린 값을 알리는지 |
| `boss_` | 보스(모세의 십계 1페이즈): 등장, 조준 빔과 비석(깔림·피하기·발판), 격파와 타이틀 복귀 |
