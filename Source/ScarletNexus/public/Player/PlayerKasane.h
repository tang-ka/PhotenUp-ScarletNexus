// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacterBase.h"
#include "PlayerKasane.generated.h"


class UBladeHandlerComponent;

UCLASS()
class SCARLETNEXUS_API APlayerKasane : public APlayerCharacterBase
{
	GENERATED_BODY()

public:
	APlayerKasane();

protected:
	virtual void BeginPlay() override;

	virtual void BasicAttack() override;
	virtual void PsychicAttack() override;
	virtual void BackStepAttack() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UBladeHandlerComponent* GetBladeHandlerComp() const { return BladeHandlerComp; }
	
protected:
	virtual void OnMontageStarted(UAnimMontage* Montage) override;
	virtual void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;

private:
#pragma region Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBladeHandlerComponent> BladeHandlerComp;
#pragma endregion
};
