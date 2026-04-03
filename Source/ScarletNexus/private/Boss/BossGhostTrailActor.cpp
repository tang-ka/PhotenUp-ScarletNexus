// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss/BossGhostTrailActor.h"
#include "Components/SkeletalMeshComponent.h"

ABossGhostTrailActor::ABossGhostTrailActor()
{
    PrimaryActorTick.bCanEverTick = true;

    GhostMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GhostMesh"));
    RootComponent = GhostMesh;
    GhostMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GhostMesh->SetCastShadow(false);
    GhostMesh->bNoSkeletonUpdate = true;
}

void ABossGhostTrailActor::InitGhost(USkeletalMeshComponent* SourceMesh, UMaterialInterface* GhostMaterial, float Lifetime)
{
    if (!SourceMesh || !GhostMaterial || !SourceMesh->GetSkeletalMeshAsset())
    {
        UE_LOG(LogTemp, Warning, TEXT("[GhostTrail] InitGhost 실패 - NULL"));
        Destroy();
        return;
    }

    FadeDuration = Lifetime;

    GhostMesh->SetSkeletalMesh(SourceMesh->GetSkeletalMeshAsset());
    GhostMesh->SetRelativeTransform(SourceMesh->GetRelativeTransform());
    GhostMesh->SetAnimInstanceClass(nullptr);

    // 포즈 스냅샷으로 복사
    FPoseSnapshot Snapshot;
    SourceMesh->SnapshotPose(Snapshot);

    // 메시 초기화를 위해 한 번 틱 허용
    GhostMesh->bNoSkeletonUpdate = false;
    GhostMesh->InitAnim(true);

    // 본 트랜스폼 직접 복사
    TArray<FTransform>& EditableBones = GhostMesh->GetEditableComponentSpaceTransforms();
    const TArray<FTransform>& SourceBones = SourceMesh->GetComponentSpaceTransforms();
    if (EditableBones.Num() == SourceBones.Num())
    {
        EditableBones = SourceBones;
        GhostMesh->MarkRenderTransformDirty();
        GhostMesh->MarkRenderDynamicDataDirty();
    }

    // 스켈레톤 업데이트 중단
    GhostMesh->bNoSkeletonUpdate = true;
    GhostMesh->SetComponentTickEnabled(false);

    // 머티리얼 적용
    const int32 NumMats = GhostMesh->GetNumMaterials();
    for (int32 i = 0; i < NumMats; i++)
    {
        UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(GhostMaterial, this);
        MID->SetScalarParameterValue(TEXT("Opacity"), InitialOpacity);
        GhostMesh->SetMaterial(i, MID);
        GhostMIDs.Add(MID);
    }

    SetActorScale3D(SourceMesh->GetOwner()->GetActorScale3D());

    UE_LOG(LogTemp, Log, TEXT("[GhostTrail] InitGhost 완료 - 머티리얼 슬롯: %d, 본 수: %d"),
        NumMats, EditableBones.Num());
}

void ABossGhostTrailActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FadeTimer += DeltaTime;
    const float Alpha = FMath::Clamp(1.f - (FadeTimer / FadeDuration), 0.f, 1.f);

    for (UMaterialInstanceDynamic* MID : GhostMIDs)
    {
        if (MID) MID->SetScalarParameterValue(TEXT("Opacity"), Alpha * InitialOpacity);
    }

    if (FadeTimer >= FadeDuration)
    {
        Destroy();
    }
}