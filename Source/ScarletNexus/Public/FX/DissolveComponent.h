// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DissolveComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UDissolveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDissolveComponent();
	
	// 디졸브 시작 (Duration초에 걸쳐 사라짐)
	UFUNCTION(BlueprintCallable, Category="Dissolve")
	void StartDissolve(float Duration = 1.5f);
	
	// 머터리얼 파라미터 이름
	UPROPERTY(EditAnywhere, Category="Dissolve")
	FName DissolveParamName = TEXT("DissolveAmount");
	
	// 디졸브 완료 후 액터 Destroy 체크
	UPROPERTY(EditAnywhere, Category="Dissolve")
	bool bDestroyOnComplete = true;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynMatList;
	
	UPROPERTY(EditAnywhere, Category = "Dissolve")
	TObjectPtr<UMaterialInterface> DissolveMaterial;
	
	bool bDissolving = false;
	float DissolveElapsed = 0.f;
	float DissolveDuration = 1.5f;
	
	void CreateDynamicMaterials();
};
