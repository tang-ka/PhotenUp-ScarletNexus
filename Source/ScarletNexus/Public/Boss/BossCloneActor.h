// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossCloneActor.generated.h"



class UCapsuleComponent;
class USkeletalMeshComponent;
 
UCLASS()
class SCARLETNEXUS_API ABossCloneActor : public AActor
{
	GENERATED_BODY()
 
public:
	ABossCloneActor();
 
	// 돌진 초기화 (스폰 후 호출)
	void InitRush(const FVector& InDirection, float InSpeed, float InDistance,
		float InDamage, float InDamageRadius, float InKnockback);
 
	// 돌진 시작 (준비 완료 후 호출)
	void StartRush();
 
	// 돌진 완료 여부
	bool IsRushComplete() const { return bRushComplete; }
 
	virtual void Tick(float DeltaTime) override;
 
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CapsuleComp;
 
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> MeshComp;
 
private:
	// 돌진 파라미터
	FVector RushDirection = FVector::ZeroVector;
	FVector StartLocation = FVector::ZeroVector;
	float RushSpeed = 3500.f;
	float RushDistance = 1200.f;
	float Damage = 200.f;
	float DamageRadius = 150.f;
	float KnockbackForce = 50.f;
 
	// 상태
	bool bRushing = false;
	bool bRushComplete = false;
	bool bDamageApplied = false;
 
	// 소멸 타이머
	float DestroyTimer = 0.f;
	float DestroyDelay = 0.2f;
 
	void ApplyRushDamage();
};