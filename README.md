# Scarlet Nexus - Kasane Action

> 스칼렛 넥서스의 카사네 전투를 재현한 3인 팀 프로젝트

<!-- TODO: 블레이드 공격 + PK 투척 대표 GIF 1개 -->

<table>
  <tr><td><b>기간</b></td><td>2026.03 ~ 2026.04 (1개월)</td></tr>
  <tr><td><b>인원</b></td><td>3인</td></tr>
  <tr><td><b>엔진</b></td><td>Unreal Engine 5.7 / C++</td></tr>
  <tr><td><b>담당</b></td><td>플레이어 캐릭터 전반 · 블레이드 · 콤보 · 사이코키네시스 · 타게팅</td></tr>
</table>

---

## 목차

- [코드 가이드](#코드-가이드)
  - [1. 블레이드 시스템](#1-블레이드-시스템)
  - [2. 입력 버퍼 기반 콤보](#2-입력-버퍼-기반-콤보)
  - [3. 사이코키네시스와 타게팅](#3-사이코키네시스와-타게팅)
- [그 외 담당](#그-외-담당)
- [팀 담당 범위](#팀-담당-범위)
- [실행](#실행)

---

## 코드 가이드

### 1. 블레이드 시스템

카사네 주변을 공전하는 블레이드를 오브젝트 풀로 관리하고, 상태 기계로 대기 · 공격 · 복귀를 전환한다.
매 공격마다 스폰/파괴하지 않도록 시작 시 풀을 한 번만 생성하고 이후 활성/비활성 전환만 수행한다.

**상태 전이** — `Inactive` → `Idle` → `Attack` → `Return` → `Idle`

- `Idle` — 공전 반경 · 부유 진폭 · 회전축 기울기를 파라미터화해 각 블레이드에 위상 오프셋을 부여, 동일 로직으로 서로 다른 궤적을 만든다
- `Attack` — 발사 원점 · 방향 · 최대 거리 · 속도를 받아 직선 비행, 부채꼴 각도로 분산 발사
- `Return` — 도달 지점을 P0, 플레이어를 P1로 두고 **2차 베지어 곡선**으로 복귀. 제어점의 좌우 방향과 높이를 난수화해 여러 블레이드가 겹쳐 보이지 않도록 했다. 끝점 P1은 매 프레임 갱신되므로 이동 중에도 추적된다

**타격 처리** — 블레이드 충돌은 개별 `AKasaneBlade`가 델리게이트로 방송하고, `UBladeHandlerComponent`가 한곳에서 받아 데미지 · 카메라 쉐이크 · 데미지 위젯 표시를 처리한다. 개별 블레이드가 게임 로직을 알지 않도록 분리했다.

**히트스탑** — 적중 시 `TimeDilation`을 낮추고 타이머로 복원. 시간 배율과 지속 시간을 분리해 노출했다.

| 역할 | 파일 |
|---|---|
| 블레이드 상태 기계 · 궤적 계산 · 히트스탑 | [`KasaneBlade.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/Private/Player/Weapon/KasaneBlade.cpp) |
| 풀 생성 · 공격 패턴 실행 · 타격 중계 | [`BladeHandlerComponent.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/Private/Player/Component/BladeHandlerComponent.cpp) |
| 상태 · 패턴 열거형, 파라미터 정의 | [`KasaneBlade.h`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/Public/Player/Weapon/KasaneBlade.h) |

---

### 2. 입력 버퍼 기반 콤보

콤보 입력이 애니메이션 재생 중에 씹히지 않도록 입력 · 행동 판정 · 콤보 진행을 세 컴포넌트로 분리했다.

| 컴포넌트 | 책임 |
|---|---|
| `UInputBufferComponent` | 입력을 타임스탬프와 함께 큐에 적재. 유효 시간이 지난 입력은 자동 폐기하고, 동일 입력이 짧은 간격으로 들어오면 무시 |
| `UActionManagerComponent` | 현재 행동 상태(`Idle` / `Attacking` / `Dashing` / `Staggered` / `PKHolding` 등)를 보유하고 전이 가능 여부를 판정. 공격 · 이동 · 대시 가능 여부를 질의 형태로 제공 |
| `UComboComponent` | 콤보 트리를 순회하며 다음 공격을 결정. 일정 시간 입력이 없으면 초기화 |

**콤보 트리는 `UComboAttackDataAsset` 링크 구조**로 구성했다. 각 노드가 자신의 몽타주와 "다음 입력 타입 → 다음 노드" 맵을 들고 있어, 콤보를 추가할 때 C++ 수정 없이 에셋만 연결하면 된다. 루트 노드는 입력 타입별로 매핑되어 있고, `TSoftObjectPtr`로 참조해 필요 시점에 로드한다.

**콤보 허용 구간은 애님 노티파이 스테이트**로 열고 닫는다. 애니메이터가 몽타주 타임라인에서 직접 구간을 조정할 수 있어, 코드 수정 없이 감각을 조율할 수 있다.

| 역할 | 파일 |
|---|---|
| 입력 큐 · 만료 처리 · 연타 방지 | [`InputBufferComponent.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/Private/Player/Component/InputBufferComponent.cpp) |
| 행동 상태 전이 · 허가 판정 | [`ActionManagerComponent.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/Private/Player/Component/ActionManagerComponent.cpp) |
| 콤보 트리 순회 · 리셋 | [`ComboComponent.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/Private/Player/Component/ComboComponent.cpp) |
| 콤보 노드 데이터 구조 | [`ComboAttackDataAsset.h`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/Public/Data/ComboAttackDataAsset.h) |
| 콤보 구간 노티파이 | [`AnimNotifyState_ComboWindow.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/Private/Player/Animation/AnimNotifyState_ComboWindow.cpp) |

---

### 3. 사이코키네시스와 타게팅

**타게팅** — 락온 없이도 조준이 자연스럽게 붙도록 Soft / Hard 두 단계로 나눴다.

- 감지 구체 안의 후보를 일정 주기로 갱신하고, **거리 · 시야각 · 화면 중심 근접도의 가중합**으로 점수를 매겨 Soft Target을 자동 선정
- 락온 시 Hard Target으로 고정, 해제 시 다시 Soft로 복귀
- PK 대상 후보는 별도 목록으로 관리하며, 선정된 대상은 CustomDepth로 외곽선 하이라이트
- 가중치와 갱신 주기를 에디터에 노출하고, 디버그 드로우로 감지 범위와 후보를 시각화

**사이코키네시스** — 오브젝트를 띄워 들고 있다가 대상에게 던진다.

- 홀드 시 오브젝트의 물리를 끄고 플레이어 머리 위로 보간 이동, 놓으면 목표를 향해 발사
- 발사 속도는 시작점 · 목표점 · 속도로부터 역산해 궤적을 맞춘다
- **기본 공격이 적중한 직후 일정 시간 내에 PK를 시작하면 StrongThrow로 전환** — 오브젝트가 베지어 곡선을 그리며 머리 위로 솟구친 뒤 발사되고, 데미지 · 카메라 쉐이크 · 히트스탑 강도가 모두 강화된다
- 적중 데미지는 `IDamageable` 인터페이스로 전달하고, 데미지 수치 위젯은 델리게이트로 방송해 UI가 구독

| 역할 | 파일 |
|---|---|
| 후보 평가 · Soft/Hard 전환 · 하이라이트 | [`PlayerPerceptionComponent.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/private/Player/Component/PlayerPerceptionComponent.cpp) |
| 홀드 · 투척 · StrongThrow · 히트 처리 | [`PsychokinesisComponent.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/private/Player/Component/PsychokinesisComponent.cpp) |
| PK 대상 오브젝트 · 충돌 방송 | [`PKObject.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/Private/PK/PKObject.cpp) |

---

## 그 외 담당

- **플레이어 캐릭터 베이스** — 컴포넌트 조립 및 입력 바인딩, 카사네 파생 클래스 ([`PlayerCharacterBase.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/private/Player/PlayerCharacterBase.cpp))
- **MVVM HUD** — `AScarletPlayerHUD`가 위젯 생성과 ViewModel 바인딩을 담당하는 조립자 역할. 파티 가입/탈퇴, 보스 등록 시 위젯을 동적으로 연결 ([`ScarletPlayerHUD.cpp`](https://github.com/tang-ka/POTENUP-ScarletNexus/blob/d5b4b807c90b07b8d95944b7b409ba5346646b19/Source/ScarletNexus/Private/Player/Widget/ScarletPlayerHUD.cpp))
- **KasaneAnimInstance** — 이동/상태 연동 애님 인스턴스
- **애님 노티파이** — 블레이드 콜리전 활성화, 블레이드 Glow 머티리얼 전환

## 팀 담당 범위

| 파트 | 담당 |
|---|---|
| 플레이어 캐릭터 · 블레이드 · 콤보 · 사이코키네시스 · 타게팅 | 🟢 본인 |
| 보스 AI (StateTree) · 적 캐릭터 | ⚪ 팀원 |
| 파티 멤버 AI · FX · 레벨 구성 | ⚪ 팀원 |

## 실행

Unreal Engine 5.7 필요. `ScarletNexus.uproject` 우클릭 → Generate Visual Studio project files → 빌드 후 실행.
