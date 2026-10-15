# WinApi_MetalSlug

Win32 API와 GDI만으로 만든 메탈슬러그 3 모작이다(2022). 엔진(레벨·오브젝트·컴포넌트·리소스 매니저), 스테이지 1 일부, 애니메이션 툴을 직접 만들었고, 2026년에 코드 리뷰와 리팩토링을 거쳤다.

대표 기능은 세 가지다.

- **픽셀 충돌**: 지형을 색으로 칠한 충돌 맵을 월드 좌표로 직접 조회해 바닥(빨강)·벽(초록)을 판정한다 (`05.Component/Collider/PixelCollider`)
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

창 크기 1200×800, 60 FPS. 타이틀(캐릭터 선택)에서 시작한다.

| 화면 | 키 | 동작 |
| --- | --- | --- |
| 타이틀 | ← → / C / F1 | 캐릭터 선택 표시 / 시작 / 애니메이션 툴 |
| 스테이지 | ← → | 이동 (화면 오른쪽 45%를 넘으면 카메라를 민다) |
| | ↑ / ↓ | 위 보기 / 앉기 (공중에서는 아래 보기) |
| | X / C / Z | 점프 / 사격 (적이 가까우면 칼) / 수류탄 |
| 게임 오버 | C를 2초 | 타이틀로 |
| 애니메이션 툴 | 패널 버튼, F1 | 조작법은 화면 오른쪽 아래에 나온다. F4로 타이틀 |

Debug 빌드에는 디버그 키가 있다(Release에서는 동작하지 않는다): 스테이지에서 Q 충돌체 표시, R 충돌 맵 표시, V 무적, F 승리 포즈, W A S D 순간 이동, ENTER 애니메이션 툴.

## 애니메이션 파일 (.anim)

`OutputFile\bin\content\animation\` 아래에 있다. UTF-16LE 텍스트이고, 애니메이션 이름 = 파일 이름 = 게임에서 `Play`하는 이름이다. 엔진과 툴이 같은 코드(`CAnimData::ReadFile/WriteFile`)로 읽고 쓴다. 게임은 파일마다 한 번만 읽어 공유한다.

## 자동 테스트

```
powershell -ExecutionPolicy Bypass -File tests\run_tests.ps1 -Config Debug     # -Filter "s9_*", -Build
```

`Client.exe --test <시나리오> --out <폴더>`로 창 없이 입력을 재생하고, 고정 DT·고정 난수로 결과를 검사한다. 시나리오 47개(리팩토링 단계별 결함 재현 포함). 문법은 [tests/README.md](tests/README.md), 사람이 볼 점검표는 [docs/manual-checklist.md](docs/manual-checklist.md).

## 폴더 구조

```
Client/Game/
  00.Header       공용 정의 (def.h: enum·상수, struct.h: Vec2·프레임, func.h)
  01.Engine       메인 루프 (tick → render → 이벤트), 60 FPS 제한
  02.Manager      키, 시간, 레벨, 충돌, 이벤트(생성·삭제·레벨 전환), 경로, 리소스, 카메라, UI, 자동 테스트
  03.Level        타이틀·스테이지·게임 오버·애니메이션 툴
  04.Object       플레이어, 몬스터, 총알·이펙트, 배경, UI
  05.Component    애니메이터, 충돌체, 픽셀 충돌, 리지드바디, AI(FSM)
  06.Resource     텍스처, 애니메이션 데이터
  07.MonsterState 몬스터 상태 (상태 없는 공용 객체)
tests/            자동 테스트 실행기와 시나리오
docs/             수동 점검표
tools/            미초기화 멤버 검사 (clang-tidy)
OutputFile/bin/content/  animation(.anim), level(카메라 경로·몬스터 배치), texture(저장소 밖)
```

## 리팩토링 기록

- 태그 `original-2022`: 2022년 원본, `refactor-1st`: 1차 정리
- 브랜치 `refactor/second-pass`: 코드 리뷰(MS-R01~R51) 뒤 2차 리팩토링. 단계마다 커밋이 나뉘어 있다 — 수명·메모리, 게임플레이 버그, 프레임 독립, 월드 좌표 통일, 픽셀 충돌, 리소스 공유, 몬스터 데이터화·FSM, 플레이어 분해, 애니메이션 툴, 정리
