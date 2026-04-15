// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/PlayerPerceptionComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Interface/DamageableHelper.h"
#include "Interface/PKInteractable.h"
#include "Player/PlayerCharacterBase.h"


UPlayerPerceptionComponent::UPlayerPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PerceptionAngleCos = FMath::Cos(FMath::DegreesToRadians(PerceptionAngleDeg));
}

void UPlayerPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitDetectionSphere();

	GetWorld()->GetTimerManager().SetTimer(
		SoftTargetUpdateTimer,
		this,
		&UPlayerPerceptionComponent::UpdatePerception,
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

	DeactivateLockOn();
}

void UPlayerPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDrawDebug)
	{
		AActor* CurrentTarget = GetCurrentTarget();

		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), SoftTargetRadius, 30, FColor::Green);

		if (GetCurrentTarget())
		{
			FVector CircleYAxis = FVector::RightVector;
			FVector CircleZAxis = FVector::UpVector;

			if (const APlayerCharacterBase* OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner()))
			{
				if (const UCameraComponent* CameraComp = OwnerCharacter->GetCameraComp())
				{
					CircleYAxis = CameraComp->GetRightVector();
					CircleZAxis = CameraComp->GetUpVector();
				}
			}

			FColor CircleColor = bIsLockedOn ? FColor::Red : FColor::Green;

			// billboard 형태로 타겟 위치에 원 그리기 (카메라 화면을 향하도록)
			DrawDebugCircle(GetWorld(),
			                CurrentTarget->GetActorLocation(),
			                80.f, 30, CircleColor,
			                false, -1.f, 0, 4,
			                CircleYAxis, CircleZAxis, false);
		}

		if (GetPsychokinesisTarget())
		{
			FVector CircleYAxis = FVector::RightVector;
			FVector CircleZAxis = FVector::UpVector;

			if (const APlayerCharacterBase* OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner()))
			{
				if (const UCameraComponent* CameraComp = OwnerCharacter->GetCameraComp())
				{
					CircleYAxis = CameraComp->GetRightVector();
					CircleZAxis = CameraComp->GetUpVector();
				}
			}

			// billboard 형태로 타겟 위치에 원 그리기 (카메라 화면을 향하도록)
			DrawDebugCircle(GetWorld(),
			                GetPsychokinesisTarget()->GetActorLocation(),
			                80.f, 30, FColor::Purple,
			                false, -1.f, 0, 4,
			                CircleYAxis, CircleZAxis, false);
		}
	}
}

#if WITH_EDITOR
void UPlayerPerceptionComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif


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

	if (HasSoftTarget())
	{
		HardTarget = SoftTarget;
	}

	// ViewModel → Widget에 HardTarget 변경을 알림
	OnHardTargetChanged.Broadcast(HardTarget.Get());
}

void UPlayerPerceptionComponent::DeactivateLockOn()
{
	bIsLockedOn = false;

	if (HardTarget.IsValid())
	{
		HardTarget.Reset();
	}

	// ViewModel → Widget에 HardTarget 해제를 알림
	OnHardTargetChanged.Broadcast(nullptr);
}

void UPlayerPerceptionComponent::LockOnToTarget(AActor* Target)
{
	if (!IsValid(Target))
	{
		return;
	}

	HardTarget = Target;
	bIsLockedOn = true;

	// 직접 타겟 지정 시 브로드캐스트
	OnHardTargetChanged.Broadcast(Target);
}

void UPlayerPerceptionComponent::SetActivePsychokinesisTargetUpdate(bool bIsActivate)
{
	// 픽업 시작(false) → 현재 PKTarget 하이라이트 즉시 해제
	if (!bIsActivate)
	{
		SetPKTargetHighlight(PsychokinesisTarget.Get(), false);
	}
	bNeedPsychokinesisTargetUpdate = bIsActivate;
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

	DetectionSphere = NewObject<USphereComponent>(Owner, TEXT("DetectionSphere"));

	DetectionSphere->SetupAttachment(GetOwner()->GetRootComponent());
	DetectionSphere->SetGenerateOverlapEvents(false);
	DetectionSphere->RegisterComponent();

	DetectionSphere->SetSphereRadius(SoftTargetRadius);
	DetectionSphere->SetCollisionProfileName(TEXT("PlayerDetection"));
	DetectionSphere->SetGenerateOverlapEvents(true);
	DetectionSphere->SetHiddenInGame(true);

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &UPlayerPerceptionComponent::OnBeginOverlap);
	DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &UPlayerPerceptionComponent::OnEndOverlap);

	TArray<AActor*> InitialOverlaps;
	DetectionSphere->GetOverlappingActors(InitialOverlaps);

	for (AActor* Actor : InitialOverlaps)
	{
		if (DamageableHelpers::IsDamageable(Actor))
		{
			if (ACharacter* Character = Cast<ACharacter>(Actor))
			{
				CandidateSoftTargets.Add(Character);
			}
		}

		if (Actor->GetClass()->ImplementsInterface(UPKInteractable::StaticClass()))
		{
			CandidatePsychokinesisTargets.Add(Actor);
		}
	}
}

void UPlayerPerceptionComponent::UpdatePerception()
{
	// 죽었거나 유효하지 않은 적을 후보 목록에서 제거
	CandidateSoftTargets.RemoveAll([](const TWeakObjectPtr<AActor>& Candidate)
	{
		return !Candidate.IsValid() || DamageableHelpers::IsDead(Candidate.Get());
	});

	// 락온 타겟이 사망하거나 유효하지 않으면 락온 해제
	if (bIsLockedOn && (!HardTarget.IsValid() || DamageableHelpers::IsDead(HardTarget.Get())))
	{
		DeactivateLockOn();
	}

	if (!bIsLockedOn)
	{
		SoftTarget = EvaluateCandidates(CandidateSoftTargets);
	}
	
	if (bNeedPsychokinesisTargetUpdate)
	{
		// 변경된 경우에만 브로드캐스트 (불필요한 이벤트 억제)
		AActor* NewPKTarget = EvaluateCandidates(CandidatePsychokinesisTargets, 0.1f, 0.2f, 0.7f);
		if (NewPKTarget != PsychokinesisTarget.Get())
		{
			SetPKTargetHighlight(PsychokinesisTarget.Get(), false); // 이전 타겟 하이라이트 해제
			SetPKTargetHighlight(NewPKTarget, true);                // 새 타겟 하이라이트 활성화
			PsychokinesisTarget = NewPKTarget;
			OnPsychokinesisTargetChanged.Broadcast(NewPKTarget);
		}
	}
}

AActor* UPlayerPerceptionComponent::EvaluateCandidates(const TArray<TWeakObjectPtr<AActor>>& Candidates,
                                                       float InDistWeight,
                                                       float InAngleWeight,
                                                       float InScreenWeight) const
{
	// 음수(센티넬)이면 멤버 변수 값을 사용
	if (InDistWeight < 0.f || InAngleWeight < 0.f || InScreenWeight < 0.f)
	{
		InDistWeight = DistWeight;
		InAngleWeight = AngleWeight;
		InScreenWeight = ScreenWeight;
	}
	else
	{
		// 양수면 가중치 합이 1이 되도록 정규화
		const float Total = InDistWeight + InAngleWeight + InScreenWeight;
		InDistWeight /= Total;
		InAngleWeight /= Total;
		InScreenWeight /= Total;
	}

	if (Candidates.Num() == 0)
	{
		return nullptr;
	}

	const auto* Owner = Cast<APlayerCharacterBase>(GetOwner());
	const FVector MyLocation = Owner->GetActorLocation();
	const FVector Forward = Owner->GetCameraComp()->GetForwardVector();

	TArray<TPair<AActor*, float>> PreFiltered;
	PreFiltered.Reserve(Candidates.Num());

	for (auto& Candidate : Candidates)
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
		if (CosAngle < PerceptionAngleCos)
		{
			continue;
		}

		// 거리 점수 계산(0~1, 가까울수록 높음)
		const float DistScore = 1.0f - FMath::Clamp(Distance / SoftTargetRadius, 0.f, 1.f);
		// 각도 점수 계산(0~1, 정면에 가까울수록 높음)
		const float AngleScore = FMath::Clamp(CosAngle, 0.f, 1.f);
		const float PreScore = (InDistWeight * DistScore) + (InAngleWeight * AngleScore);

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
		float TotalScore = PreFiltered[i].Value + ScreenScore * InScreenWeight;

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

void UPlayerPerceptionComponent::SetPKTargetHighlight(AActor* Target, bool bHighlight) const
{
	if (!IsValid(Target)) return;

	if (UStaticMeshComponent* MeshComp = Target->FindComponentByClass<UStaticMeshComponent>())
	{
		MeshComp->SetRenderCustomDepth(bHighlight);
	}
}

void UPlayerPerceptionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                const FHitResult& SweepResult)
{	
	if (DamageableHelpers::IsDamageable(OtherActor) && !DamageableHelpers::IsDead(OtherActor))
	{
		if (ACharacter* Character = Cast<ACharacter>(OtherActor))
		{
			CandidateSoftTargets.Add(Character);
		}
	}

	if (OtherActor->GetClass()->ImplementsInterface(UPKInteractable::StaticClass()))
	{
		CandidatePsychokinesisTargets.Add(OtherActor);
	}
}

void UPlayerPerceptionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CandidateSoftTargets.Remove(OtherActor);
	if (GetSoftTarget() == OtherActor)
	{
		SoftTarget.Reset();
	}

	if (GetHardTarget() == OtherActor)
	{
		HardTarget.Reset();
	}

	CandidatePsychokinesisTargets.Remove(OtherActor);
	if (GetPsychokinesisTarget() == OtherActor)
	{
		SetPKTargetHighlight(OtherActor, false); // 범위 이탈 시 하이라이트 해제
		PsychokinesisTarget.Reset();
		// PKTarget이 범위를 벗어나면 즉시 nullptr 브로드캐스트
		OnPsychokinesisTargetChanged.Broadcast(nullptr);
	}
}
