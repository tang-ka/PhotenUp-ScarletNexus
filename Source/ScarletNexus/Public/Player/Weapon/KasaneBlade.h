// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KasaneBlade.generated.h"

class USphereComponent;
class AKasaneBlade;
class UPrimitiveComponent;

UENUM(BlueprintType)
enum class EBladeState : uint8
{
	None UMETA(DisplayName = "None"),
	Inactive UMETA(DisplayName = "Inactive"), // 블레이드가 비활성화된 상태 (풀에서 대기 중)
	Idle UMETA(DisplayName = "Idle"), // 블레이드가 대기 상태로 플레이어 주변에 떠 있는 상태
	Attack UMETA(DisplayName = "Attack"), // 블레이드가 공격 중인 상태
	Return UMETA(DisplayName = "Return"), // 블레이드가 공격 후 플레이어에게 돌아오는 상태
};

UENUM(BlueprintType)
enum class EBladeAttackPattern : uint8
{
	None,
	A1, // 방사형 투척
	A2, // 추후
	A3 // 추후
};

// 블레이드 충돌 시 호출되는 델리게이트 (OnSphereOverlap 인자 전달)
DECLARE_DELEGATE_SevenParams(FOnBladeHit,
	AKasaneBlade*,
	UPrimitiveComponent*,
	AActor*,
	UPrimitiveComponent*,
	int32,
	bool,
	const FHitResult&);

UCLASS()
class SCARLETNEXUS_API AKasaneBlade : public AActor
{
	GENERATED_BODY()

public:
	AKasaneBlade();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	bool IsMoving() const;

	void SetActive(bool bActivate);
	void SetActiveCollision(bool bActivate) const;
	void SetDefaultState(EBladeState NewDefault) { DefaultState = NewDefault; }
	
	void ChangeBladeState(EBladeState NewState);
	EBladeState GetBladeState() const { return CurState; }
	EBladeState GetDefaultState() const { return DefaultState; }

	void Init(AActor* InOwner, int32 Index);
	void InitForIdle(int32 InIdleBladeCount);

	void LaunchAttack(EBladeAttackPattern InPattern,
	                  FVector InOrigin, FVector InDirection,
	                  float InMaxDistance, float InSpeed);

	// === Critical ===
	void SetCanCritical(bool bValue) { bCanCritical = bValue; }
	bool GetCanCritical() const { return bCanCritical; }

	// === 충돌 델리게이트 (BladeHandler에서 바인딩) ===
	FOnBladeHit OnBladeHit;

	// === HitStop ===
	void TriggerHitStop();
	
private:
	void BeginInactive();
	void BeginIdle();
	void BeginAttack();
	void BeginReturn();

	void TickIdle(float DeltaTime);
	
	void TickAttack(float DeltaTime);
	// void TickAttackA1(float DeltaTime);
	
	void TickReturn(float DeltaTime);

	// === HitStop ===
	void EndHitStop();
	
public:
	int32 ReturnIdleIndex = -1;
	
private:
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, Category = "Blade|State")
	EBladeState CurState = EBladeState::Inactive;

	UPROPERTY(VisibleAnywhere, Category = "Blade|State")
	EBladeAttackPattern CurAttackPattern = EBladeAttackPattern::None;
	
	UPROPERTY(VisibleAnywhere, Category = "Blade|State")
	EBladeState DefaultState = EBladeState::Idle;

	// === Critical ===
	UPROPERTY(VisibleAnywhere, Category = "Blade|Critical")
	bool bCanCritical = false;
	
#pragma region Idle
	UPROPERTY(EditDefaultsOnly, Category = "Blade|Idle")
	float OrbitRadius = 80.f; // 캐릭터 중심으로부터의 거리

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Idle")
	float OrbitHeightOffset = 60.f; // 캐릭터보다 얼마나 위에 배치할지

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Idle")
	float OrbitSpeed = 90.f; // 공전 속도 (도/초)

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Idle")
	float BobAmplitude = 8.f; // 위아래 부유 진폭

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Idle")
	float BobSpeed = 2.5f; // 부유 속도

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Idle")
	float AxisTiltSpeed = 10.f; // 회전축 방향 변화 속도 (도/초)

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Idle")
	float AxisTiltAmount = 20.f; // 회전축 최대 기울기 (도) 

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Idle")
	float FollowInterpSpeed = 5.f; // Owner 추종 보간 속도
#pragma endregion

	// --- AttackA1 파라미터 ---
	FVector AttackOrigin;        // 발사 시점 위치
	FVector AttackDirection;     // 발사 방향 (정규화)
	float AttackMaxDistance;     // 최대 비행 거리
	float AttackSpeed;           // 비행 속도
	float AttackElapsed;         // 경과 시간
	
#pragma region return
	UPROPERTY(EditDefaultsOnly, Category = "Blade|Return")
	float ReturnSpeed = 1200.f; // 블레이드가 플레이어에게 돌아올 때의 속도

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Return")
	float ReturnHomingStrength = 8.f; // 돌아올 때 플레이어를 향해 궤도를 조정하는 정도
	
	// --- Return 파라미터 ---
	UPROPERTY(EditDefaultsOnly, Category = "Blade|Return")
	float ReturnDuration = 0.6f;         // 복귀 소요 시간

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Return")
	float ReturnCurveStrength = 300.f;   // 곡선 휘어짐 강도

	UPROPERTY(EditDefaultsOnly, Category = "Blade|Return")
	float ReturnHeightVariance = 150.f;  // 위아래 불규칙 정도
	
	// --- Return 런타임 ---
	FVector P0;       // 시작점 (최대거리 도달 위치)
	FVector CP0;       // 제어점 (곡선 방향)
	FVector P1;       // 끝점 (Owner 위치 - 매 프레임 갱신)
	float ReturnElapsed;
	float ReturnCurveSide;  // +1 or -1 (좌우 방향)
	
	FVector ReturnTargetOffset;
	UPROPERTY(EditDefaultsOnly, Category = "Blade|Return")
	float ReturnArrivalRadius = 50.f; // 복귀 도착 허용 반경 (이 안에 들어오면 복귀 완료 처리)
#pragma endregion

	TWeakObjectPtr<AActor> OwnerActor;

	int32 BladeIndex = 0; // 풀에서의 인덱스 (0부터 시작)
	int32 IdleBladeCount = 1; // 총 블레이드 수
	float PhaseOffset = 0.f; // 위상 오프셋

	FVector PrevLocation = FVector::ZeroVector;

	// === HitStop ===
	UPROPERTY(EditDefaultsOnly, Category = "Blade|HitStop")
	float HitStopTimeDilation = 0.05f; // 히트스탑 시 시간 배율

	UPROPERTY(EditDefaultsOnly, Category = "Blade|HitStop")
	float HitStopDuration = 0.08f;     // 히트스탑 실제 지속 시간 (초)

	FTimerHandle HitStopTimerHandle;

#pragma region Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> BladeMesh;
#pragma endregion
};
