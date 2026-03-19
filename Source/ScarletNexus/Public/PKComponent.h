// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKInteractable.h"
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

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	
private:
	// 탐색 조준중인 타겟 PK오브젝트
	UPROPERTY()
	TObjectPtr<class APKObject> CurrentTarget;
	// 실제로 타겟팅해서 잡은 PK오브젝트
	UPROPERTY()
	TObjectPtr<class APKObject> HeldObject;
	
	// 타겟 탐색
	UFUNCTION(BlueprintCallable)
	void TraceTarget();
	// 타겟 홀드
	UFUNCTION(BlueprintCallable)
	void HoldTarget();
	UFUNCTION(BlueprintCallable)
	void ReleaseTarget();
	UFUNCTION(BlueprintCallable)
	void UseHeldTarget(const FVector& ThrowDir, float ThrowForce);
};
