// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PsychokinesisComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UPsychokinesisComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPsychokinesisComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
#pragma region Getters/Setters
	UFUNCTION(BlueprintCallable)
	bool HasPickedObject() const { return PickedObject.IsValid(); }
	
	UFUNCTION(BlueprintCallable)
	bool HasTarget() const { return ThrowTarget.IsValid(); }
	
	UFUNCTION(BlueprintCallable)
	void SetPickedObject(AActor* NewPickedObject);
	
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* NewTarget) { ThrowTarget = NewTarget; }
#pragma endregion
	
	UFUNCTION(BlueprintCallable)
	void StartHold();
	
	UFUNCTION(BlueprintCallable)
	void ReleaseHold();
	
	UFUNCTION(BlueprintCallable)
	void Throw();
	
	UFUNCTION(BlueprintCallable)
	void StrongThrow(); // 기본 공격을 맞췄을 경우 발동
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=PK, meta=(AllowPrivateAccess=true))
	TWeakObjectPtr<AActor> PickedObject;;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=PK, meta=(AllowPrivateAccess=true))
	TWeakObjectPtr<AActor> ThrowTarget;
	
	UPROPERTY(EditDefaultsOnly, Category=PK)
	float ThrowForce = 1000.f; // 던지는 힘
	
	UPROPERTY(EditDefaultsOnly, Category=PK)
	float StrongThrowForce = 1500.f; // 강한 던지는 힘 (기본 공격을 맞췄을 때)
	
	UPROPERTY(EditDefaultsOnly, Category=PK)
	float HoldTime = 2.f; // 이 시간이 지나면 던짐.
	
	FTimerHandle HoldTimerHandle;
	
	bool bHolding = false; // Hold 중인지 여부
	
	FVector HoldStartLocation;
	float HoldElapsedTime = 0.f; // Hold 시작 후 경과 시간
	
	UPROPERTY(EditDefaultsOnly, Category=PK)
	float FloatingHeight = 200.f; // 집어서 띄워지는 높이 (플레이어로부터)
	
	UPROPERTY(EditDefaultsOnly, Category=PK)
	float FloatingTime = 1.f;
};
