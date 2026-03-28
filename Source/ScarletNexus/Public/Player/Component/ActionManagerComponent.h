// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionManagerComponent.generated.h"

UENUM(BlueprintType)
enum class EActionState : uint8
{
	Idle            UMETA(DisplayName = "Idle"),
	Attacking       UMETA(DisplayName = "Attacking"),
	Dashing         UMETA(DisplayName = "Dashing"),
	Staggered       UMETA(DisplayName = "Staggered"),
	Jumping         UMETA(DisplayName = "Jumping"),
	Dead            UMETA(DisplayName = "Dead"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnActionStateChanged,
	EActionState, OldState,
	EActionState, NewState);
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UActionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UActionManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
public:
#pragma region State Management
	// ===== 상태 조회 =====
	FORCEINLINE EActionState GetCurrentState() const { return CurState; }

	// ===== 행동 허가 판단 =====
	UFUNCTION(BlueprintCallable, Category = "Action Manager")
	bool CanAttack() const;
	
	UFUNCTION(BlueprintCallable, Category = "Action Manager")
	bool CanDash() const;
	
	UFUNCTION(BlueprintCallable, Category = "Action Manager")
	bool CanJump() const;

	// --- 상태 전이 ---
	UFUNCTION(BlueprintCallable, Category = "Action Manager")
	bool TrySetState(EActionState NewState);

	UFUNCTION(BlueprintCallable, Category = "Action Manager")
	void ForceSetState(EActionState NewState);
#pragma endregion
	
#pragma region Combo Window
	FORCEINLINE bool IsComboWindowOpen() const { return bComboWindowOpen; }
	
	UFUNCTION(BlueprintCallable, Category = "Action Manager")
	void OpenComboWindow();
	
	UFUNCTION(BlueprintCallable, Category = "Action Manager")
	void CloseComboWindow();
#pragma endregion
	
private:
	bool IsValidTransition(EActionState From, EActionState To) const;
	FString GetStateName(EActionState State) const;
	
public:
	// ===== 델리게이트 =====
	UPROPERTY(BlueprintAssignable, Category = "Action Manager")
	FOnActionStateChanged OnActionStateChanged;
	
private:
	UPROPERTY(VisibleAnywhere, Category="Action Manager")
	EActionState CurState = EActionState::Idle;
	
	UPROPERTY(VisibleAnywhere, Category = "Action Manager")
	bool bComboWindowOpen = false;
};
