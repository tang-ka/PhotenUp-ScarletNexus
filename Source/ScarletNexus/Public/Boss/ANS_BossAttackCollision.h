// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_BossAttackCollision.generated.h"


UCLASS(DisplayName = "Boss Attack Collision")
class SCARLETNEXUS_API UANS_BossAttackCollision : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	// 콜리전 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FName CollisionComponentName = FName("RightFootCollision");

	// 이 구간의 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Damage = 100.f;

	// 넉백 힘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Knockback = 50.f;
	

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
};