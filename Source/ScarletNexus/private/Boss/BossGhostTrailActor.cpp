// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss/BossGhostTrailActor.h"
#include "Components/SkeletalMeshComponent.h"

ABossGhostTrailActor::ABossGhostTrailActor()
{
	PrimaryActorTick.bCanEverTick = true;

	GhostMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GhostMesh"));
	SetRootComponent(GhostMesh);

	// 물리/콜리전 비활성화
	GhostMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GhostMesh->SetSimulatePhysics(false);
	GhostMesh->bNoSkeletonUpdate = true;  // 애니메이션 업데이트 안 함 (포즈 고정)
}

void ABossGhostTrailActor::InitGhost(USkeletalMeshComponent* SourceMesh, UMaterialInterface* GhostMaterial, float Lifetime)
{
	if (!SourceMesh || !GhostMaterial) return;

	FadeDuration = Lifetime;
	FadeTimer = 0.f;

	// 메시 복사
	GhostMesh->SetSkeletalMesh(SourceMesh->GetSkeletalMeshAsset());
	GhostMesh->SetRelativeTransform(SourceMesh->GetRelativeTransform());
	GhostMesh->SetAnimInstanceClass(nullptr);
	GhostMesh->SetComponentTickEnabled(false);

	// ★ 본 트랜스폼 직접 복사
	const TArray<FTransform>& SourceBoneSpaces = SourceMesh->GetComponentSpaceTransforms();
	GhostMesh->GetEditableComponentSpaceTransforms() = SourceBoneSpaces;
	GhostMesh->bNoSkeletonUpdate = true;
	GhostMesh->MarkRenderTransformDirty();
	GhostMesh->MarkRenderDynamicDataDirty();

	// 모든 머티리얼 슬롯에 고스트 머티리얼 적용
	GhostMIDs.Empty();
	for (int32 i = 0; i < GhostMesh->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(GhostMaterial, this);
		MID->SetScalarParameterValue(FName("Opacity"), InitialOpacity);
		GhostMesh->SetMaterial(i, MID);
		GhostMIDs.Add(MID);
	}

	// 액터 스케일 복사
	if (AActor* SourceOwner = SourceMesh->GetOwner())
	{
		SetActorScale3D(SourceOwner->GetActorScale3D());
	}

	UE_LOG(LogTemp, Log, TEXT("[GhostTrail] InitGhost 완료 - 본 수: %d, 머티리얼 슬롯: %d, 위치: %s"),
		SourceBoneSpaces.Num(), GhostMesh->GetNumMaterials(), *GetActorLocation().ToString());
}

void ABossGhostTrailActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FadeTimer += DeltaTime;

	// 페이드아웃
	const float Alpha = FMath::Clamp(1.f - (FadeTimer / FadeDuration), 0.f, 1.f);
	const float CurrentOpacity = InitialOpacity * Alpha;

	for (UMaterialInstanceDynamic* MID : GhostMIDs)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(FName("Opacity"), CurrentOpacity);
		}
	}

	// 수명 끝나면 소멸
	if (FadeTimer >= FadeDuration)
	{
		Destroy();
	}
}