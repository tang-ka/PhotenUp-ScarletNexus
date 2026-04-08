// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Enemy/EnemyAttackCollision.h>
#include "CoreMinimal.h"
#include "BossAttackCollisionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UBossAttackCollisionComponent : public UEnemyAttackCollision
{
 	GENERATED_BODY()
//
// public:
// 	UBossAttackCollisionComponent();
//
// 	// 콜리전 활성화 (데미지 판정 시작)
// 	void EnableAttackCollision(float InDamage, float InKnockback = 0.f);
//
// 	// 콜리전 비활성화 (데미지 판정 종료)
// 	void DisableAttackCollision();
//
// 	// 이번 공격에서 이미 히트한 액터 초기화
// 	void ResetHitActors();
//
// protected:
// 	virtual void BeginPlay() override;
//
// 	UFUNCTION()
// 	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
// 		bool bFromSweep, const FHitResult& SweepResult);
//
// private:
// 	// 현재 공격의 데미지
// 	float CurrentDamage = 0.f;
//
// 	// 현재 공격의 넉백
// 	float CurrentKnockback = 0.f;
//
// 	// 이번 공격에서 이미 히트한 액터 목록 (중복 데미지 방지)
// 	UPROPERTY()
// 	TArray<TObjectPtr<AActor>> HitActors;
};