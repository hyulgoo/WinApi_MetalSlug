# WinApi_MetalSlug

Win32 API와 GDI만으로 만든 메탈슬러그 3 모작이다(2022). 엔진(레벨·오브젝트·컴포넌트·리소스 매니저), 스테이지 1 일부, 애니메이션 툴을 직접 만들었고, 2026년에 코드 리뷰와 리팩토링을 거쳤다. 스테이지 끝의 보스(모세의 십계)는 1페이즈까지 있다(아래 「보스」).

대표 기능은 세 가지다.

- **픽셀 충돌**: 지형을 색으로 칠한 충돌 맵을 월드 좌표로 직접 조회해 바닥(마젠타)·벽(초록)을 판정한다 (`05.Component/Collider/PixelCollider`)
- **몬스터 FSM**: 좀비 다섯 종을 데이터 표(`tZombieDesc`) 하나로 정의하고, 상태(enum)가 애니메이션과 행동을 맡는다. 공격 시점은 애니메이션 프레임 이벤트로 정한다 (`04.Object/Character/Monster`, `07.MonsterState`)
- **애니메이션 툴**: 스프라이트 시트에서 드래그로 프레임을 잘라 `.anim` 파일로 저장한다. 실행 중 시트 선택, 프레임 삭제·순서·되돌리기, 상체·하체 겹쳐 보기 (`03.Level/03.AnimationEditor`)

## 빌드

- Windows 10/11, Visual Studio 2022 ("C++를 사용한 데스크톱 개발" 워크로드, 도구 집합 v143, Windows 10 SDK)
- `WinAPI_43.sln`을 열어 `x64`의 `Debug` 또는 `Release`로 빌드한다. 외부 라이브러리는 없다
- 결과: Debug는 `OutputFile\bin_d\Client.exe`, Release는 `OutputFile\bin\Client.exe`. 둘 다 `OutputFile\bin\content\`의 데이터를 읽는다

명령줄로 빌드하려면 (VS 2022 개발자 명령 프롬프트):

```
msbuild WinAPI_43.sln -m -p:Configuration=Release -p:Platform=x64
```

## 텍스처 준비 (저장소에 없음)

스프라이트 시트(원작 그림, 약 180MB)는 저장소에 넣지 않았다. 아래 18개 BMP를 `OutputFile\bin\content\texture\`에 넣어야 화면이 보인다.

| 쓰는 곳 | 파일 |
| --- | --- |
| 배경·충돌 맵 | `BackGround.bmp` `BackGround_Back.bmp` `BackGround_Front01.bmp` `BackGround_Front02.bmp` `PixelBackGround.bmp` |
| 캐릭터·무기·이펙트 | `Eri.bmp` `Weapone.bmp` `SFX.bmp` `Soldier.bmp` `Zombie.bmp` `Helpful_NPC.bmp` `Boss01.bmp` |
| 타이틀·UI | `Select.bmp` `Select_Screen.bmp` `GAMEOVER.bmp` `HUD_HP.bmp` `HUD_WEAPONE.bmp` `NUMBER.bmp` |

빠진 텍스처가 있어도 실행은 된다. 처음 찾을 때 한 번 알리고, 투명색(마젠타) 1×1 그림으로 대신 그린다. 단, 픽셀 충돌 맵이 없으면 캐릭터가 바닥 없이 떨어진다.

## 실행과 조작

창 크기 1200×800, 60 FPS (`data\game.ini`). 타이틀(캐릭터 선택)에서 시작한다.

| 화면 | 키 | 동작 |
| --- | --- | --- |
| 타이틀 | ← → / C / F2 / F1 | 캐릭터 선택 표시 / 시작 / 보스전부터 시작 / 애니메이션 툴 |
| 스테이지 | ← → | 이동 (화면 오른쪽 45%를 넘으면 카메라를 민다) |
| | ↑ / ↓ | 위 보기 / 앉기 (공중에서는 아래 보기) |
| | X / C / Z | 점프 / 사격 (적이 가까우면 칼) / 수류탄 |
| 게임 오버 | C를 2초 | 타이틀로 |
| 애니메이션 툴 | 패널 버튼, F1 | 조작법은 화면 오른쪽 아래에 나온다. F4로 타이틀 |

Debug 빌드에는 디버그 키가 있다(Release에서는 동작하지 않는다): 스테이지에서 Q 충돌체 표시, R 충돌 맵 표시, V 무적, F 승리 포즈, W A S D 순간 이동, ENTER 애니메이션 툴.

## 보스: 모세의 십계 (1페이즈)

원작(메탈슬러그 3 미션 2)의 보스를 `Boss01.bmp` 시트로 만들었다. 체력 절반 뒤의 2페이즈(주황 돔, 더 빠른 비석)는 없고, 1페이즈 체력이 다하면 부서진다.

- **바로 시작**: 타이틀에서 F2를 누르면 처음이 아니라 보스가 나오는 자리에서 시작한다(앞의 몬스터 배치는 건너뛴다. Release에서도 된다)
- **등장**: 카메라가 스테이지 끝(레벨 파일의 `BOSS`, 카메라 x 5150)에 오면 배경의 돌무덤 자리에서 돔이 먼지 기둥 사이로 솟아오른다. 그동안은 맞지 않는다
- **공격**: 기다리다 돔이 빛기둥을 쏘면 하늘에서 가는 조준 빔이 내려와 플레이어를 따라온다(플레이어보다 느리다). 빔이 멈추고 잠시 뒤 사라지면 그 자리에 비석이 떨어지고, 박히는 순간 그 자리에 있으면 깔린다. 비석이 떨어지기 시작하고 약 2초 뒤에 다음 빔이 나온다
- **비석**: 박힌 비석은 플레이어를 옆에서 막고(걸어서도 뛰어서도), 위에서 내려오면 밟는 발판이다. 2초 뒤부터 천천히 가라앉고(33장), 총알은 한 발에 두 장·수류탄은 열 장씩 그 자리에서 바로 더 가라앉힌다. 처음 높이(504)는 점프로 넘지 못하므로 쏘아서 낮추거나 기다렸다가 뛰어오른다(윗면 약 190 이하. 발이 윗면에서 몸 높이의 2/3쯤 아래까지만 올라와도 올라선다)
- **맞는 곳**: 돔. 땅에서 옆으로 쏘면 빗나가고(받침대는 배경 쪽) 위로 쏘면 맞는다. 맞으면 원작처럼 돔이 빨갛게 깜빡인다(0.2초 동안 0.05초 간격. 밝기는 두고 색만 바꾼 시트 사본을 한 번 만들어 그 높이의 줄만 덮어 그린다: `CResMgr::GetTintedTexture`, `CAnimator::SetTint`)
- **격파**: 돔이 깨진 잔해 위로 폭발이 이어지고, 남은 빔은 사라지고 비석은 빨리 가라앉는다. 플레이어가 승리 포즈를 하고 잠시 뒤 타이틀로 돌아간다
- 코드: FSM에 등장(`APPEAR`)·기다리기(`WAIT`) 상태를 더하고 공격(`ATTACK`)·사망(`DEAD`)은 일반 몬스터와 같은 상태를 쓴다 (`CBoss`, `07.MonsterState`). 빔·비석·깔림 판정은 `04.Object/Bullet/Enemy`, 잔해는 `CBossWreck`. 비석은 `OBSTACLE` 레이어로, 픽셀 충돌이 윗면은 발판으로 찾고 옆면은 플레이어가 움직인 뒤 막는다(`CPixelCollider::BlockObstacleSide`)
- 수치: `data\boss.ini`, 클리어 뒤 흐름은 `stage.ini [clear]`, 발판·옆면 판정은 `game.ini [pixel_collider]`. 애니메이션은 `animation\Boss\` (시트에서 칸과 그림을 대조해 돔·빛기둥·잔해의 자리를 맞췄다)

## 애니메이션 파일 (.anim)

`OutputFile\bin\content\animation\` 아래에 있다. UTF-16LE 텍스트이고, 애니메이션 이름 = 파일 이름 = 게임에서 `Play`하는 이름이다. 엔진과 툴이 같은 코드(`CAnimData::ReadFile/WriteFile`)로 읽고 쓴다. 게임은 파일마다 한 번만 읽어 공유한다.

## 게임 수치 (데이터 파일)

속도·시간·크기·위치·피해·배율·화면 배치 같은 수치는 코드가 아니라 `OutputFile\bin\content\data\`의 `.ini` 파일에 있다 (`CDataMgr`가 읽는다).

| 파일 | 내용 |
| --- | --- |
| `game.ini` | 창 크기, FPS, 배경·애니메이션 배율, 상체·하체 그림 오프셋, 강체 처음 값, 픽셀 충돌(발판 포함), 디버그 키 |
| `player.ini` | 이동·점프·카메라 밀기, 몸·피격 범위, 목숨·소환·사망, 탄·폭탄, 사격 간격·자세, 총구 자리 |
| `weapon.ini` | 총알·폭탄·칼, 적 토사물·수류탄·칼, 맞은 자리 이펙트, 화면 밖 투사체를 지우는 경계 |
| `monster.ini` | 받는 피해, 좀비 공통·다섯 종, 병사, 연구원 좀비가 남기는 상자 |
| `boss.ini` | 보스(모세의 십계): 체력·맞는 범위·공격 간격, 등장, 조준 빔, 비석, 부서진 뒤 폭발 |
| `stage.ini` | 남은 시간, 떨어졌을 때, 보스를 이긴 뒤, 카메라 경로 속도·스테이지 끝, 배경, HUD 배치 |
| `title.ini` | 타이틀(선택 화면·흔들림·전환 시각)과 게임 오버 |
| `editor.ini` | 애니메이션 툴의 화면 배치·키 반복·되돌리기 수·격자 처음 값·버튼 색 |

- 형식: `[구역]` 아래에 `키 = 값`. `;`·`#` 뒤는 주석, 두 값은 `x, y`, 색은 `r, g, b`. UTF-8로 저장한다. 키마다 뜻과 단위가 파일에 주석으로 있다
- 레벨에 들어갈 때마다 다시 읽는다. 게임을 켠 채 고치고 그 레벨에 다시 들어가면 바뀐 값으로 만든다. `game.ini`의 `[window]`·`[time]`·`[scale]`만 실행할 때 한 번 읽는다
- 값이 없거나 숫자가 아니면 오류 창으로 한 번 알리고 0으로 쓴다
- 코드에 남긴 숫자: 계산 자체의 값(π, 각도·단위 변환, 0·1·절반), 그림 파일 규약(투명색 마젠타, 충돌 맵의 땅·벽 색, 32비트), 버퍼·배열 크기, `.anim` 파일 검사 한도, 자동 테스트 코드

## 자동 테스트

```
powershell -ExecutionPolicy Bypass -File tests\run_tests.ps1 -Config Debug     # -Filter "s9_*", -Build
```

`Client.exe --test <시나리오> --out <폴더>`로 창 없이 입력을 재생하고, 고정 DT·고정 난수로 결과를 검사한다. 시나리오 66개(리팩토링 단계별 결함 재현, 보스 포함). 문법은 [tests/README.md](tests/README.md), 사람이 볼 점검표는 [docs/manual-checklist.md](docs/manual-checklist.md).

## 폴더 구조

```
Client/Game/
  00.Header       공용 정의 (def.h: enum·상수, struct.h: Vec2·프레임, func.h)
  01.Engine       메인 루프 (tick → render → 이벤트), 60 FPS 제한
  02.Manager      키, 시간, 레벨, 충돌, 이벤트(생성·삭제·레벨 전환), 경로, 리소스, 카메라, UI, 자동 테스트, 게임 수치 데이터
  03.Level        타이틀·스테이지·게임 오버·애니메이션 툴
  04.Object       플레이어, 몬스터(보스 포함), 총알·이펙트, 배경, UI
  05.Component    애니메이터, 충돌체, 픽셀 충돌, 리지드바디, AI(FSM)
  06.Resource     텍스처, 애니메이션 데이터
  07.MonsterState 몬스터 상태 (상태 없는 공용 객체)
tests/            자동 테스트 실행기와 시나리오
docs/             수동 점검표
tools/            미초기화 멤버 검사 (clang-tidy)
OutputFile/bin/content/  animation(.anim), level(카메라 경로·몬스터·보스 배치), data(게임 수치 .ini), texture(저장소 밖)
```

## 리팩토링 기록

- 태그 `original-2022`: 2022년 원본, `refactor-1st`: 1차 정리
- 브랜치 `refactor/second-pass`: 코드 리뷰(MS-R01~R51) 뒤 2차 리팩토링. 단계마다 커밋이 나뉘어 있다 — 수명·메모리, 게임플레이 버그, 프레임 독립, 월드 좌표 통일, 픽셀 충돌, 리소스 공유, 몬스터 데이터화·FSM, 플레이어 분해, 애니메이션 툴, 정리
