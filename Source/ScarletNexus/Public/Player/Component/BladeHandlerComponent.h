// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BladeHandlerComponent.generated.h"


class AKasaneBlade;
class UCameraShakeBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UBladeHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBladeHandlerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	void ExecuteAttackA1();
	void SetActiveAllBladesCollision(bool bActivate);
	void SetCanCriticalAllBlades(bool bCanCritical);
	void SetAttackStateAllBlades();
	void SetDefaultStateAllBlades();

private:
	void SpawnBladePool();
	void ActivateIdleBlades();

	// 블레이드 충돌 처리 핸들러 (Blade의 OnBladeHit 델리게이트에 바인딩)
	void HandleBladeHit(AKasaneBlade* HitBlade, AActor* HitActor);

private:
#pragma region Blade Pool Settings
	UPROPERTY(EditDefaultsOnly, Category = "Blade")
	int32 PoolSize = 6; // 블레이드 풀 크기
	
	UPROPERTY(EditDefaultsOnly, Category = "Blade")
	int32 IdleBladeCount = 3; // 대기 상태로 유지할 블레이드 수
	
	UPROPERTY(EditDefaultsOnly, Category = "Blade")
	TSubclassOf<AKasaneBlade> BladeClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Blade")
	TArray<TObjectPtr<AKasaneBlade>> BladePool;
#pragma endregion 
	
#pragma region Attack A1
	UPROPERTY(EditDefaultsOnly, Category = "Blade|AttackA1")
	float A1MaxDistance = 600.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Blade|AttackA1")
	float A1SpreadAngle = 30.f;  // 부채꼴 전체 각도

	UPROPERTY(EditDefaultsOnly, Category = "Blade|AttackA1")
	float A1Speed = 2000.f;
#pragma endregion

#pragma region CameraShake
	// 블레이드 충돌 시 재생할 카메라 쉐이크
	UPROPERTY(EditDefaultsOnly, Category = "Blade|CameraShake")
	TSubclassOf<UCameraShakeBase> HitCameraShakeClass;
#pragma endregion
};
