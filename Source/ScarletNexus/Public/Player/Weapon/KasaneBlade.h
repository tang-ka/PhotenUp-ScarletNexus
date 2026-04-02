// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KasaneBlade.generated.h"

class USphereComponent;

UCLASS()
class SCARLETNEXUS_API AKasaneBlade : public AActor
{
	GENERATED_BODY()

public:
	AKasaneBlade();

protected: 
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
private:
#pragma region Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> BladeMesh;
#pragma endregion
};
