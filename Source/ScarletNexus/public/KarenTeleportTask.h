// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "StateTreeTaskBase.h"
#include "KarenTeleportTask.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EKarenTeleportType : uint8
{
	BehindPlayer  UMETA(DisplayName = "플레이어 등 뒤"),
	FrontOfPlayer UMETA(DisplayName = "플레이어 정면"),
	AbovePlayer   UMETA(DisplayName = "플레이어 머리 위"),
	Retreat       UMETA(DisplayName = "뒤로 후퇴"),
	Evade         UMETA(DisplayName = "측면 회피")
};

class SCARLETNEXUS_API KarenTeleportTask
{
public:
	KarenTeleportTask();
	~KarenTeleportTask();
};


USTRUCT()
struct SCARLETNEXUS_API FKarenTeleportTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Parameter")
	AActor* PlayerActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	EKarenTeleportType TeleportType = EKarenTeleportType::BehindPlayer;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (EditCondition = "TeleportType != EKarenTeleportType::AbovePlayer"))
	float TeleportDistance = 300.0f; 

	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (EditCondition = "TeleportType == EKarenTeleportType::AbovePlayer"))
	float TeleportHeight = 600.0f; 
};

//USTRUCT(meta = (DisplayName = "Karen Tactical Teleport"))
//struct SCARLETNEXUS_API FKarenTeleportTask : public FStateTreeTaskBase
// {
// 	GENERATED_BODY()
// 	using FInstanceDataType = FKarenTeleportTaskInstanceData;
//
// protected:
// 	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
// 	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
// };