// BossGhostTrailActor.h
// 보스 돌진 시 잔상(고스트 트레일) 액터
// SetLeaderPoseComponent로 현재 포즈를 받은 뒤 N프레임 후 고정 → 페이드아웃 → 소멸

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossGhostTrailActor.generated.h"

UCLASS()
class SCARLETNEXUS_API ABossGhostTrailActor : public AActor
{
	GENERATED_BODY()

public:
	ABossGhostTrailActor();

	virtual void Tick(float DeltaTime) override;

	// 보스 메시 포즈 복사 → 고스트 생성
	void InitGhost(USkeletalMeshComponent* SourceMesh, UMaterialInterface* GhostMaterial, float InLifetime);

private:
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> GhostMesh;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> GhostMIDs;

	// 페이드 관련
	float FadeTimer = 0.f;
	float FadeDuration = 0.5f;
	float InitialOpacity = 0.5f;

	// LeaderPose 해제 카운트다운 (-1 = 비활성/이미 고정됨)
	int32 FreezeCountdown = -1;
};