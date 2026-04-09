// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/STTask_BossDeath.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Boss/BossCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"


EStateTreeRunStatus FSTTask_BossDeath::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss) return EStateTreeRunStatus::Failed;

	Data.Timer = 0.f;
	Data.bDeathStarted = true;
	Data.StartLocation = Boss->GetActorLocation();

	if (UCharacterMovementComponent* MoveComp = Boss->GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	// 여기서 사망 몽타주 재생
	if (ABossCharacterBase* BossBase = Cast<ABossCharacterBase>(Boss))
	{
		TArray<UNiagaraComponent*> NiagaraComps;
		Boss->GetComponents<UNiagaraComponent>(NiagaraComps);
		for (UNiagaraComponent* NC : NiagaraComps)
		{
			if (NC)
			{
				NC->Deactivate();
				NC->DestroyComponent();
			}
		}
		
		if (BossBase->DeathMontage)
		{
			Data.DeathMontageLength = Boss->PlayAnimMontage(BossBase->DeathMontage);
			UE_LOG(LogTemp, Warning, TEXT("[BossDeath] 몽타주 길이: %.2f"), Data.DeathMontageLength);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[BossDeath] DeathMontage가 nullptr!"));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[BossDeath] 사망 State 진입 — 몽타주 재생"));
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossDeath::Tick(
	FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& Data = Context.GetInstanceData(*this);
	ACharacter* Boss = Cast<ACharacter>(Data.ContextActor);
	if (!Boss) return EStateTreeRunStatus::Failed;

	Data.Timer += DeltaTime;

	// 1. 몽타주 시간 대기
	if (Data.Timer < Data.DeathMontageLength)
	{
		return EStateTreeRunStatus::Running;
	}

	// 2. 포즈 고정
	Boss->GetMesh()->bPauseAnims = true;

	// 3. 1.5초 대기 후 가라앉기
	float PostMontageTimer = Data.Timer - Data.DeathMontageLength;

	if (PostMontageTimer < 1.5f)
	{
		return EStateTreeRunStatus::Running;
	}

	// 4. 가라앉기
	if (UCapsuleComponent* Capsule = Boss->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	const float SinkTimer = PostMontageTimer - 1.5f;

	if (SinkTimer < DeathDuration)
	{
		const float Alpha = FMath::Clamp(SinkTimer / DeathDuration, 0.f, 1.f);
		const float EasedAlpha = FMath::InterpEaseIn(0.f, 1.f, Alpha, 2.f);
		const float NewZ = Data.StartLocation.Z - (SinkDepth * EasedAlpha);

		FVector NewLocation = Data.StartLocation;
		NewLocation.Z = NewZ;
		Boss->SetActorLocation(NewLocation);
	}

	if (SinkTimer >= DeathDuration + HideDelay)
	{
		Boss->SetActorHiddenInGame(true);
		UGameplayStatics::OpenLevel(Boss->GetWorld(), FName("CutsceneLevel3"));
		UE_LOG(LogTemp, Warning, TEXT("[BossDeath] 사망 완료 — 액터 숨김"));
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}
 
