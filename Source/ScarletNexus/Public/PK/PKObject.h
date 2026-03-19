// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PKInteractable.h"
#include "PKObject.generated.h"

UENUM()
enum class EPKObjectState : uint8
{
	CanBePickedUp = 0, // 집기 전
	IsHeld,			   // 집힌 상태
	IsUsed			   // 사용된 상태 (Thrown, Crumple, Ride)
};

UENUM()
enum class EPKObjectType : uint8
{
	Throwable = 0,	// 던질 수 있는
	Crumplable,		// 찌그러트릴 수 있는
	Rideable		// 탑승할 수 있는 (버스 등)
};

// PK : PsychokinesisObject (염력으로 집을 수 있는 물체)
UCLASS()
class SCARLETNEXUS_API APKObject : public AActor, public IPKInteractable
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
	
	// <물리>
	// 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxComp;
	// 질량
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Mass = 100.f;
	// 속력 (던져질 때 날아가는 경우)
	FVector Velocity = FVector::ZeroVector;
	
	// 피염력체 타입
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EPKObjectType ObjectType = EPKObjectType::Throwable;
	
	// 피염력체 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EPKObjectState ObjectState = EPKObjectState::CanBePickedUp;
	
	// 사용되었는지 체크
	bool bUsedObject = false;
	
	// PK 인터페이스 구현
	virtual bool CanBePickeduped_Implementation() const override;
	virtual void OnPKPickuped_Implementation() override;
	virtual void OnPKReleased_Implementation() override;
	virtual void OnPKThrown_Implementation(const FVector& ThrowDir, float ThrowForce) override;
};
