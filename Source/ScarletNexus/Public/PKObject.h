// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PKObject.generated.h"

UENUM()
enum class EPKObjectState : uint8
{
	CanBePickedUp = 0, // 집기 전
	IsHeld,			   // 집힌 상태
	IsThrown		   // 던져진 상태
};

// PK : PsychokinesisObject (염력으로 집을 수 있는 물체)
UCLASS()
class SCARLETNEXUS_API APKObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APKObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// 외형
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> StaticMeshComp;
	
	// 물리
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxComp;
	
	// 피염력체 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EPKObjectState ObjectState = EPKObjectState::CanBePickedUp;
	
	// 집을 수 있는 상태
	UFUNCTION(BlueprintCallable)
	bool CanBePickedUped() const { return ObjectState == EPKObjectState::CanBePickedUp; }
	// 집어 올려지기
	UFUNCTION(BlueprintCallable)
	void OnPKPickUped();
	// 다시 놓아지기 (집기 해제)
	UFUNCTION(BlueprintCallable)
	void OnPKReleased();
	// 집혀졌을 때 던져지기
	UFUNCTION(BlueprintCallable)
	void OnPKThrown(const FVector& ThrowDir, float ThrowForce);
};
