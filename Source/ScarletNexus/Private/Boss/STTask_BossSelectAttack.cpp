


#include "Boss/STTask_BossSelectAttack.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
 
EStateTreeRunStatus FSTTask_BossSelectAttack::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.bMontageFinished = false;
 
	if (!BossConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("[BossSelectAttack] BossConfig이 설정되지 않았습니다."));
		return EStateTreeRunStatus::Failed;
	}
 
	// 현재 페이즈에서 사용 가능한 패턴 가져오기 (누적 구조)
	TArray<FBossAttackPattern> AvailablePatterns = BossConfig->GetAvailablePatterns(InstanceData.CurrentPhase);
	if (AvailablePatterns.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossSelectAttack] Phase %d에 사용 가능한 패턴이 없습니다."),
			static_cast<int32>(InstanceData.CurrentPhase));
		return EStateTreeRunStatus::Failed;
	}
 
	// 가중치 기반으로 패턴 선택
	InstanceData.SelectedPatternIndex = SelectPatternByWeight(
		AvailablePatterns,
		InstanceData.DistanceToPlayer,
		InstanceData.bPlayerInSight
	);
 
	if (InstanceData.SelectedPatternIndex == INDEX_NONE)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// 선택된 패턴의 몽타주 재생
	const FBossAttackPattern& SelectedPattern = AvailablePatterns[InstanceData.SelectedPatternIndex];
 
	ACharacter* BossCharacter = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossCharacter)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	UE_LOG(LogTemp, Log, TEXT("[BossSelectAttack] 패턴 선택: %s (Phase: %d, Distance: %.0f)"),
		*SelectedPattern.AttackTag.ToString(),
		static_cast<int32>(InstanceData.CurrentPhase),
		InstanceData.DistanceToPlayer);
 
	// 몽타주가 없으면 바로 완료 (테스트용)
	if (!SelectedPattern.AttackMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossSelectAttack] 몽타주 없음 - 바로 완료"));
		return EStateTreeRunStatus::Succeeded;
	}
 
	UAnimInstance* AnimInstance = BossCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// 몽타주 재생
	AnimInstance->Montage_Play(SelectedPattern.AttackMontage, 1.f);
 
	// 몽타주가 끝날 때까지 Running 유지
	return EStateTreeRunStatus::Running;
}
 
EStateTreeRunStatus FSTTask_BossSelectAttack::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
 
	ACharacter* BossCharacter = Cast<ACharacter>(InstanceData.ContextActor);
	if (!BossCharacter)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	UAnimInstance* AnimInstance = BossCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return EStateTreeRunStatus::Failed;
	}
 
	// 몽타주가 끝나면 공격 완료
	if (!AnimInstance->IsAnyMontagePlaying())
	{
		return EStateTreeRunStatus::Succeeded;
	}
 
	return EStateTreeRunStatus::Running;
}
 
void FSTTask_BossSelectAttack::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.SelectedPatternIndex = INDEX_NONE;
}
 
int32 FSTTask_BossSelectAttack::SelectPatternByWeight(
	const TArray<FBossAttackPattern>& Patterns,
	float DistanceToPlayer,
	bool bPlayerInSight) const
{
	TArray<float> AdjustedWeights;
	AdjustedWeights.Reserve(Patterns.Num());
 
	float TotalWeight = 0.f;
 
	for (const FBossAttackPattern& Pattern : Patterns)
	{
		float Weight = Pattern.SelectionWeight;
 
		// 거리 기반 보정
		if (Pattern.AttackRange <= 300.f)
		{
			// 근접 패턴: 가까우면 가중치 UP, 멀면 DOWN
			if (DistanceToPlayer <= Pattern.AttackRange * 1.2f)
			{
				Weight *= 2.f;
			}
			else
			{
				Weight *= 0.2f;
			}
		}
		else
		{
			// 원거리 패턴: 멀수록 가중치 UP
			if (DistanceToPlayer > 400.f)
			{
				Weight *= 1.5f;
			}
		}
 
		AdjustedWeights.Add(Weight);
		TotalWeight += Weight;
	}
 
	if (TotalWeight <= 0.f)
	{
		return INDEX_NONE;
	}
 
	// 가중치 룰렛 선택
	float RandomValue = FMath::FRandRange(0.f, TotalWeight);
	float Accumulator = 0.f;
 
	for (int32 i = 0; i < AdjustedWeights.Num(); ++i)
	{
		Accumulator += AdjustedWeights[i];
		if (RandomValue <= Accumulator)
		{
			return i;
		}
	}
 
	return Patterns.Num() - 1;
}