// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/AttackType.h"
#include "GameFramework/Character.h"
#include "Interface/Damageable.h"
#include "PlayerCharacterBase.generated.h"

class UComboAttackDataAsset;
class UComboComponent;
class UInputBufferComponent;
class UActionManagerComponent;
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
class SCARLETNEXUS_API APlayerCharacterBase : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	APlayerCharacterBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region IDamageable Interface
	virtual bool ReceiveDamage_Implementation(FDamageInfo DamageInfo) override;
	virtual int GetHP_Implementation() const override;
	virtual float GetHPPercent_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
#pragma endregion

#pragma region Component Getters
	FORCEINLINE UCameraComponent* GetCameraComp() const { return CameraComp; }
	FORCEINLINE USpringArmComponent* GetSpringArmComp() const { return SpringArmComp; }
	FORCEINLINE UPlayerStatsComponent* GetStatsComp() const { return StatsComp; }
	FORCEINLINE UPlayerStateComponent* GetStateComp() const { return StateComp; }
	FORCEINLINE UPlayerPerceptionComponent* GetPerceptionComp() const { return PerceptionComp; }
	FORCEINLINE UPsychokinesisComponent* GetPsychokinesisComp() const { return PsychokinesisComp; }
	FORCEINLINE UInputBufferComponent* GetInputBufferComp() const { return InputBufferComp; }
	FORCEINLINE UActionManagerComponent* GetActionManagerComp() const { return ActionManagerComp; }
	FORCEINLINE UComboComponent* GetComboComp() const { return ComboComp; }
#pragma endregion

protected:
#pragma region Input Action Functions
	void OnMoveInput(const FInputActionValue& Value);
	void OnLookInput(const FInputActionValue& Value);
	void OnJumpInput(const FInputActionValue& Value);
	void OnCompleteJumpInput(const FInputActionValue& Value);
	void OnDodgeInput(const FInputActionValue& Value);

	void OnBasicAttackInput(const FInputActionValue& Value);
	void OnPsychokinesisInput(const FInputActionValue& Value);
	void OnCompletePsychokinesisInput(const FInputActionValue& Value);

	void OnBackAttackInput(const FInputActionValue& Value);
	void OnLockOnInput(const FInputActionValue& Value);
#pragma endregion

#pragma region Attack Functions
	virtual void BasicAttack();
	virtual void PsychicAttack();
	virtual void BackStepAttack();

	void ExecuteAttack(EAttackType AttackType);
	void PlayCurrentAttackMontage();
	virtual void PlayAttackMontage(const UComboAttackDataAsset* AttackDataAsset);
	// Attacking 상태에서 몽타주가 끝났을 때만 콤보 리셋 및 상태 변경
	UFUNCTION()
	void OnMontageEdnded(UAnimMontage* Montage, bool bInterrupted);
public:
	void TryConsumeBufferedAttack();
#pragma endregion

private:
	void Move(const FVector2D& Direction);
	void Look(const FVector2D& LookVector);
	void Dash(FVector& InDashDirection);
	void ResetDash();
	void LockOnToggle();

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

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_BackAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_LockOn;
#pragma endregion

private:
#pragma region Dash Properties
	bool bIsDashing{false}; // 상태 중심
	bool bCanDash{true}; // 상태 및 쿨타임 중심
	bool bNeedAdjustLookForward{false}; // 대쉬 방향이 이동 방향과 다를 때 true
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputBufferComponent> InputBufferComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UActionManagerComponent> ActionManagerComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UComboComponent> ComboComp;
#pragma endregion
};
