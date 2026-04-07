// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_DeactivateDamage.generated.h"

UCLASS(DisplayName="Deactivate Attack Collision")
class SCARLETNEXUS_API UAnimNotify_DeactivateDamage : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual FString GetNotifyName_Implementation() const override {return TEXT("DeactivateCollision");}
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
