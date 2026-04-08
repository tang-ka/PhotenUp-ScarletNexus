// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BossAttackCollisionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UBossAttackCollisionComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UBossAttackCollisionComponent();

	void EnableAttackCollision(float InDamage, float InKnockback = 0.f);
	void DisableAttackCollision();
	void ResetHitActors();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	   bool bFromSweep, const FHitResult& SweepResult);

private:
	float CurrentDamage = 0.f;
	float CurrentKnockback = 0.f;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActors;
};