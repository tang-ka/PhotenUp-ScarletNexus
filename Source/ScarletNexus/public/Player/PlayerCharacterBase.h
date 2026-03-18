// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacterBase.generated.h"

class UCameraComponent;
class USpringArmComponent;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

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
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void StopJumpingAction(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);
	void BasicAttack(const FInputActionValue& Value);
	void Psychokinesis(const FInputActionValue& Value);
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
#pragma endregion
};
