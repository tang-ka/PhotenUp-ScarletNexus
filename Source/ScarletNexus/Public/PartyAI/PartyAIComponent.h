// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StateTreeComponent.h"
#include "PartyAIComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCARLETNEXUS_API UPartyAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPartyAIComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 에디터에서 ST_PartyCharacter 할당 or 경로 설정 Set
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<class UStateTree> PartyCharacterST;
	
	// AIController에서 Possess 시 호출
	void StartAI();

private:
	UPROPERTY()
	TObjectPtr<UStateTreeComponent> StateTreeComp;
};
