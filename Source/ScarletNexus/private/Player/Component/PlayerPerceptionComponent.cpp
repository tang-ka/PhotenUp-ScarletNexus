// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/PlayerPerceptionComponent.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Interface/DamageableHelper.h"


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
	
	// DistWeight와 AngleWeight의 합이 1이 되도록 강제
	if (DistWeight + AngleWeight + ScreenWeight != 1.f)
	{
		float Total = DistWeight + AngleWeight + ScreenWeight;
		DistWeight /= Total;
		AngleWeight /= Total;
		ScreenWeight /= Total;
	}
}

void UPlayerPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), SoftTargetRadius, 30, FColor::Green);
		
		if (SoftTarget.IsValid())
		{
			DrawDebugSphere(GetWorld(), SoftTarget->GetActorLocation(), 30.f, 12, FColor::Red,
				false, -1.f, 0, 5.f);
		}
	}
}

void UPlayerPerceptionComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

AActor* UPlayerPerceptionComponent::GetCurrentTarget() const
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
	if (DetectionSphere)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner || !Owner->GetRootComponent())
	{
		return;
	}

	DetectionSphere = NewObject<USphereComponent>(this, TEXT("DetectionSphere"));

	DetectionSphere->SetupAttachment(GetOwner()->GetRootComponent());
	DetectionSphere->RegisterComponent();

	DetectionSphere->SetSphereRadius(SoftTargetRadius);
	DetectionSphere->SetCollisionProfileName(TEXT("PlayerDetection"));
	DetectionSphere->SetGenerateOverlapEvents(true);
	DetectionSphere->SetHiddenInGame(true);

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &UPlayerPerceptionComponent::OnBeginOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &UPlayerPerceptionComponent::OnEndOverlap);
}

void UPlayerPerceptionComponent::UpdateSoftTarget()
{
	UE_LOG(LogTemp, Warning, TEXT("Updating Soft Target"));
	
	SoftTarget = EvaluateCandidates();
}

AActor* UPlayerPerceptionComponent::EvaluateCandidates() const
{
	if (CandidateSoftTargets.Num() == 0)
	{
		return nullptr;
	}

	AActor* Owner = GetOwner();
	const FVector MyLocation = GetOwner()->GetActorLocation();
	const FVector Forward = GetOwner()->GetActorForwardVector();

	TArray<TPair<AActor*, float>> PreFiltered;
	PreFiltered.Reserve(CandidateSoftTargets.Num());

	for (auto& Candidate : CandidateSoftTargets)
	{
		if (!Candidate.IsValid())
		{
			continue;
		}

		FVector Direction = (Candidate->GetActorLocation() - MyLocation);
		float Distance = Direction.Size();
		FVector DirectionNorm = Direction.GetSafeNormal();

		const float CosAngle = FVector::DotProduct(Forward, DirectionNorm);

		// 방향이 범위 밖인 후보는 제외 (뒤에 있는 후보도 자동 제외)
		if (CosAngle < CosMinAngle)
		{
			continue;
		}

		// 거리 점수 계산(0~1, 가까울수록 높음)
		const float DistScore = 1.0f - FMath::Clamp(Distance / SoftTargetRadius, 0.f, 1.f);
		// 각도 점수 계산(0~1, 정면에 가까울수록 높음)
		const float AngleScore = FMath::Clamp(CosAngle, 0.f, 1.f);
		const float PreScore = (DistWeight * DistScore) + (AngleWeight * AngleScore);

		PreFiltered.Emplace(Candidate.Get(), PreScore);
	}

	if (PreFiltered.Num() == 0)
	{
		return nullptr;
	}

	PreFiltered.Sort([](const TPair<AActor*, float>& A, const TPair<AActor*, float>& B)-> bool
	{
		return A.Value > B.Value; // 내림차순 정렬
	});

	const int32 TopN = FMath::Min(PreFiltered.Num(), TopCount); // 상위 5개 후보만 실제로 스크린 좌표로 변환해서 비교
	
	// 스크린 중앙이랑 가까운 후보 선정 (최적화 : 상위 N개 후보만 실제로 스크린 좌표로 변환해서 비교)
	AActor* BestCandidate = nullptr;
	float BestScore = -FLT_MAX;
	
	for (int32 i = 0; i < TopN; i++)
	{
		float ScreenScore = CalcScreenCenterScore(PreFiltered[i].Key);
		float TotalScore = PreFiltered[i].Value + ScreenScore * ScreenWeight;
		
		if (TotalScore > BestScore)
		{
			BestScore = TotalScore;
			BestCandidate = PreFiltered[i].Key;
		}
	}
	
	return BestCandidate;
}

float UPlayerPerceptionComponent::CalcScreenCenterScore(AActor* Target) const
{
	if (!Target)
	{
		return 0.f;
	}

	APawn* PawnOwner = Cast<APawn>(GetOwner());
	APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());
	if (!PC)
	{
		return 0.f;
	}

	FVector2D ScreenPos;
	bool bOnScreen = PC->ProjectWorldLocationToScreen(Target->GetActorLocation(), ScreenPos);
	if (!bOnScreen)
	{
		return 0.f;
	}

	int32 ViewportX, ViewportY;
	PC->GetViewportSize(ViewportX, ViewportY);
	
	FVector2D ScreenCenter(ViewportX * 0.5f, ViewportY * 0.5f);
	float MaxDist = ScreenCenter.Size();
	float DistFromCenter = FVector2D::Distance(ScreenPos, ScreenCenter);
	float Score = FMath::Clamp(1.0f - (DistFromCenter / MaxDist), 0.f, 1.f); // 중앙에 가까울수록 점수 높음

	return Score;
}

void UPlayerPerceptionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                const FHitResult& SweepResult)
{
	if (DamageableHelpers::IsDamageable(OtherActor))
	{
		if (ACharacter* Character = Cast<ACharacter>(OtherActor))
		{
			CandidateSoftTargets.Add(Character);
		}
	}

	// if (OtherActor->GetClass()->ImplementsInterface(UPsychokinesisTargetable::StaticClass()))
	// {
	// 	CandidatePsychokinesisTargets.Add(OtherActor);
	// }
}

void UPlayerPerceptionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CandidateSoftTargets.Remove(OtherActor);
	if (GetSoftTarget() == OtherActor)
	{
		SoftTarget.Reset();
	}
}
