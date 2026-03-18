// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacterBase.generated.h"

class UPsychokinesisComponent;
class UPlayerPerceptionComponent;
class UPlayerStateComponent;
class UPlayerStatsComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;

struct FInputActionValue;

/**
 * Basic Movement, Look, Jump, Psychokinesis, Dodge
 * Basic Attack is not implemented yet, but the input action is set up for it.
 */
UCLASS()
class SCARLETNEXUS_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacterBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
#pragma region Input Action Functions
	void OnMoveInput(const FInputActionValue& Value);
	void OnLookInput(const FInputActionValue& Value);
	void OnJumpInput(const FInputActionValue& Value);
	void OnCompleteJumpInput(const FInputActionValue& Value);
	void OnDodgeInput(const FInputActionValue& Value);
	void OnBasicAttackInput(const FInputActionValue& Value);
	void OnPsychokinesisInput(const FInputActionValue& Value);
#pragma endregion
	
private:
	void Move(const FVector2D& Direction);
	void Look(const FVector2D& LookVector);
	void Dash();
	void ResetDash();

private:
#pragma region Dash
	bool bIsDashing{false}; // 상태 중심
	bool bCanDash{true};	// 상태 및 쿨타임 중심
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
#pragma endregion
	
protected:
#pragma region Input Action Properties
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Player;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Dodge;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_BasicAttack;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Psychokinesis;
#pragma endregion
	
private:
#pragma region Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerStatsComponent> StatsComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerStateComponent> StateComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerPerceptionComponent> PerceptionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPsychokinesisComponent> PsychokinesisComp;
#pragma endregion
};
