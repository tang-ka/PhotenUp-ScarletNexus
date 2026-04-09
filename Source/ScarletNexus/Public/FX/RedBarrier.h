// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RedBarrier.generated.h"

class UBoxComponent;

UCLASS()
class SCARLETNEXUS_API ARedBarrier : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARedBarrier();
	
	// 배리어 On/Off
	UFUNCTION(BlueprintCallable)
	void ActivateBarrier(bool bActivate);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BlockingVolume;
	
	UPROPERTY(EditAnywhere, Category="Barrier")
	TObjectPtr<UMaterialInterface> BarrierMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynMatInst;
	
	// 배리어 색상 (HDR)
	UPROPERTY(EditAnywhere, Category="Barrier")
	FLinearColor BarrierColor = FLinearColor(1.f, 0.05f, 0.02f);
	
	UPROPERTY(EditAnywhere, Category="Barrier")
	float EmissiveIntensity = 5.f;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
