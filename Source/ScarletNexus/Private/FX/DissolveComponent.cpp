// Fill out your copyright notice in the Description page of Project Settings.


#include "FX/DissolveComponent.h"


// Sets default values for this component's properties
UDissolveComponent::UDissolveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


// Called when the game starts
void UDissolveComponent::BeginPlay()
{
	Super::BeginPlay();
	CreateDynamicMaterials();
}


// Called every frame
void UDissolveComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!bDissolving) return;
	
	DissolveElapsed += DeltaTime;
	const float alpha = FMath::Clamp(DissolveElapsed / DissolveDuration, 0.f, 1.f);
	
	for (UMaterialInstanceDynamic* dynMat : DynMatList)
	{
		if (dynMat)
		{
			dynMat->SetScalarParameterValue(DissolveParamName, alpha);
		}
	}
	
	if (alpha >= 1.f)
	{
		bDissolving = false;
		SetComponentTickEnabled(false);
		
		if (bDestroyOnComplete)
		{
			if (AActor* owner = GetOwner())
			{
				owner->Destroy();
			}
		}
	}
}

void UDissolveComponent::StartDissolve(float Duration)
{
	DissolveDuration = FMath::Max(Duration, 0.1f);
	DissolveElapsed = 0.f;
	bDissolving = true;
	SetComponentTickEnabled(true);
	
	// 콜리전 즉시 해제 (죽은 뒤 충돌 방지)
	AActor* owner = GetOwner();
	if (!owner) return;
	
	owner->SetActorEnableCollision(false);
	
	// 디졸브 머터리얼로 교체
	if (!DissolveMaterial) return;
	
	DynMatList.Empty();
	TArray<UMeshComponent*>	meshes;
	owner->GetComponents<UMeshComponent>(meshes);
	
	for (UMeshComponent* mesh : meshes)
	{
		for (int32 i = 0; i < mesh->GetNumMaterials(); ++i)
		{
			UMaterialInstanceDynamic* dynMat = UMaterialInstanceDynamic::Create(DissolveMaterial, this);
			mesh->SetMaterial(i, dynMat);
			DynMatList.Add(dynMat);
		}
	}
}

void UDissolveComponent::CreateDynamicMaterials()
{
	AActor* owner = GetOwner();
	if (!owner) return;
	
	// 오너의 모든 MeshComponent에서 Dynamic Material Instance 생성
	TArray<UMeshComponent*>	meshes;
	owner->GetComponents<UMeshComponent>(meshes);
	
	for (UMeshComponent* mesh : meshes)
	{
		for (int32 i = 0; i < mesh->GetNumMaterials(); ++i)
		{
			UMaterialInterface* material = mesh->GetMaterial(i);
			if (!material) return;
			
			UMaterialInstanceDynamic* dynMat = mesh->CreateAndSetMaterialInstanceDynamic(i);
			if (dynMat)
			{
				DynMatList.Add(dynMat);
			}
		}
	}
}
