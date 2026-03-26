// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerPerceptionComponent.generated.h"


class USphereComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UPlayerPerceptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerPerceptionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#pragma region Getters
	bool HasSoftTarget() const { return SoftTarget.IsValid(); }
	AActor* GetSoftTarget() const { return SoftTarget.IsValid() ? SoftTarget.Get() : nullptr; }
	AActor* GetHardTarget() const { return HardTarget.IsValid() ? HardTarget.Get() : nullptr; }
	AActor* GetCurrentTarget() const;
	AActor* GetPsychokinesisTarget() const { return PsychokinesisTarget.IsValid() ? PsychokinesisTarget.Get() : nullptr; }
#pragma endregion

#pragma region Lock-On
	UFUNCTION(BlueprintCallable)
	void ActivateLockOn();
	UFUNCTION(BlueprintCallable)
	void DeactivateLockOn();
	UFUNCTION(BlueprintCallable)
	bool IsLockedOnActivate() const { return bIsLockedOn; }
#pragma endregion

private:
	void InitDetectionSphere();
	void UpdatePerception();
	AActor* EvaluateCandidates(const TArray<TWeakObjectPtr<AActor>>& Candidates,
	                           float InDistWeight = -1.f,
	                           float InAngleWeight = -1.f,
	                           float InScreenWeight = -1.f) const;
	float CalcScreenCenterScore(AActor* Target) const;	

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                    bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	FTimerHandle SoftTargetUpdateTimer;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> DetectionSphere;

	UPROPERTY(EditAnywhere, Category = "Perception|SoftTarget Settings")
	float SoftTargetRadius{2000.f};

	UPROPERTY(EditAnywhere, Category = "Perception|SoftTarget Settings", meta = (ClampMin = "0.0", ClampMax = "179.0"))
	float PerceptionAngleDeg{60.f};
	float PerceptionAngleCos;
	
	UPROPERTY(EditAnywhere, Category = "Perception|SoftTarget Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DistWeight{0.3f};
	
	UPROPERTY(EditAnywhere, Category = "Perception|SoftTarget Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AngleWeight{0.3f};
	
	UPROPERTY(EditAnywhere, Category = "Perception|SoftTarget Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ScreenWeight{0.4f};
	
	UPROPERTY(EditAnywhere, Category = "Perception|SoftTarget Settings")
	float SoftTargetUpdateInterval{0.15f};
	
	UPROPERTY(EditAnywhere, Category = "Perception|SoftTarget Settings")
	int32 TopCount{5};

#pragma region Target
	TWeakObjectPtr<AActor> SoftTarget;
	TWeakObjectPtr<AActor> HardTarget;
	TWeakObjectPtr<AActor> PsychokinesisTarget;
	
	TArray<TWeakObjectPtr<AActor>> CandidateSoftTargets;
	TArray<TWeakObjectPtr<AActor>> CandidatePsychokinesisTargets;
#pragma endregion
	
	bool bIsLockedOn{false};
	
	UPROPERTY(EditAnywhere, Category = "Perception|Debug")
	bool bDrawDebug{true};
};
