// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashSkillComponent.generated.h"

class ACharacter;
class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCARLETNEXUS_API UDashSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDashSkillComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartDash(FVector InDashDirection, bool bAdjustLook = false);

private:
	void EndDash();
	void ResetDash();
	void ProcessDash(float DeltaTime);
	void AdjustLookForward();

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCameraComponent> CachedCameraComp;

	bool bIsDashing{false};
	bool bCanDash{true};
	bool bNeedAdjustLookForward{false};

	FVector DashDirection{};
	FVector DashVelocity{};
	float DashTimeRemaining{0.f};
	FTimerHandle DashCooldownTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDistance{600.f};

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDuration{0.2f};

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashCooldown{0.2f};

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDampingFactor{0.5f};
};
