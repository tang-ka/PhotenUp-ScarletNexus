// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PKComponent.generated.h"

// Psycokinesis Component (염력 스킬 컴포넌트)
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UPKComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPKComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	// 속성
	// 탐색 거리
	UPROPERTY(EditAnywhere)
	float TraceDistance = 1000.f;
	// 잡은 오브젝트 띄울 거리
	UPROPERTY(EditAnywhere)
	float HoldDistance = 250.f;
	// 던지는 힘
	UPROPERTY(EditAnywhere)
	float HoldForce = 1500.f;
	// 탐색 반경
	UPROPERTY(EditAnywhere)
	float TraceRadius = 100.f;
	// AI용 시야각 필터 (dot product 기준, 기본 0.7 약 45도)
	UPROPERTY(EditAnywhere)
	float FOVDotThreshold = 0.7f;
	
	// 상태
	// 탐색 조준중인 타겟 PK오브젝트
	UPROPERTY()
	TObjectPtr<class APKObject> CurrentTarget;
	// 실제로 타겟팅해서 잡은 PK오브젝트
	UPROPERTY()
	TObjectPtr<class APKObject> HeldObject;
	// 잡은 오브젝트의 물리 컴포넌트
	UPROPERTY()
	TObjectPtr<class UPrimitiveComponent> HeldPrimitive = nullptr;
	
	// 인터페이스
	// 타겟 탐색
	UFUNCTION(BlueprintCallable)
	void TraceTarget();
	// 타겟 홀드
	UFUNCTION(BlueprintCallable)
	void HoldTarget();
	// 타겟 해제
	UFUNCTION(BlueprintCallable)
	void ReleaseTarget();
	// 타겟 사용 (던지기/찌그러트리기/올라타기)
	UFUNCTION(BlueprintCallable)
	void UseHeldTarget(const FVector& ThrowDir, float ThrowForce);
	// 현재 잡은 PK오브젝트가 있는지 체크
	UFUNCTION(BlueprintCallable)
	bool IsHoldingPKObject() { return HeldObject != nullptr; }
	
	// 내부 헬퍼
	// owner의 시선 방향 반환 (플레이어 = 카메라, AI = 액터 전방)
	FVector GetOwnerForwardVector() const;
	// 잡은 오브젝트를 홀드 위치로 부드럽게 이동
	void UpdateHeldObjectPosition(float DeltaTime);
	// 홀드 목표 위치 계산 (오너 앞 약간 위)
	FVector GetHoldTargetLocation() const;
	
	// 타이머
	// 트레이싱 타이머
	FTimerHandle TraceTimerHandle;
	// 트레이싱 딜레이
	float TraceDelay = 0.5f;
};
