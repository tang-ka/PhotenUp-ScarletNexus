// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossGhostTrailActor.generated.h"

UCLASS()
class SCARLETNEXUS_API ABossGhostTrailActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABossGhostTrailActor();
	
	virtual void Tick(float DeltaTime) override;
	
	// 보스메시 포즈 복사 -> 고스트생성
	void InitGhost(USkeletalMeshComponent* SourceMesh, UMaterialInterface* GhostMaterial, float Lifetime);
	
	
private:
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> GhostMesh;
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> GhostMIDs;
	
	float FadeTimer = 0.f;
	float FadeDuration = 2.0f;
	float InitialOpacity = 0.5f;	
	int32 FreezeCountdown = -1;  // -1이면 비활성
};
