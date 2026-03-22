// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PartyMemberBase.generated.h"

UCLASS()
class SCARLETNEXUS_API APartyMemberBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APartyMemberBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// State Tree 실행 콤포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UPartyAIComponent> PartyAIComp;

	// 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 500.f;
};
