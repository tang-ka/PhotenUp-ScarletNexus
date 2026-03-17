// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "KarenAIController.generated.h"

/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API AKarenAIController : public AAIController
{
	GENERATED_BODY()
	
	public:
	AKarenAIController();
	
	protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class UStateTreeComponent* StateTreeComponent;
	
};
