// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/PlayerPerceptionComponent.h"

#include "Components/SphereComponent.h"


UPlayerPerceptionComponent::UPlayerPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPlayerPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitDetectionSphere();
	
	GetWorld()->GetTimerManager().SetTimer(
		SoftTargetUpdateTimer,
		this,
		&UPlayerPerceptionComponent::UpdateSoftTarget,
		SoftTargetUpdateInterval,
		true
	);
}

void UPlayerPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

ACharacter* UPlayerPerceptionComponent::GetCurrentTarget() const
{
	if (bIsLockedOn)
	{
		return GetHardTarget();
	}
	
	return GetSoftTarget();
}

void UPlayerPerceptionComponent::ActivateLockOn()
{
	bIsLockedOn = true;
}

void UPlayerPerceptionComponent::DeactivateLockOn()
{
	bIsLockedOn = false;
}

void UPlayerPerceptionComponent::InitDetectionSphere()
{
	DetectionSphere = NewObject<USphereComponent>(this, TEXT("DetectionSphere"));
	
	DetectionSphere->SetupAttachment(GetOwner()->GetRootComponent());
	DetectionSphere->RegisterComponent();
	
	DetectionSphere->SetSphereRadius(SoftTargetRadius);
	DetectionSphere->SetCollisionProfileName(TEXT("PlayerDetection"));
	DetectionSphere->SetGenerateOverlapEvents(true);
	DetectionSphere->SetHiddenInGame(true);
	
}

void UPlayerPerceptionComponent::UpdateSoftTarget()
{
	UE_LOG(LogTemp, Warning, TEXT("Updating Soft Target"));
}

ACharacter* UPlayerPerceptionComponent::FindBestSoftTarget() const
{
	return nullptr;
}

void UPlayerPerceptionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void UPlayerPerceptionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
