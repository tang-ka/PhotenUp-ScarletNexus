// Fill out your copyright notice in the Description page of Project Settings.


#include "KarenTeleportTask.h"
//#include "StateTreeExecutionContext.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"


KarenTeleportTask::KarenTeleportTask()
{
	
}

KarenTeleportTask::~KarenTeleportTask()
{
	
}

// EStateTreeRunStatus FKarenTeleportTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
// {
// 	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
// 	AActor* KarenActor = Cast<AActor>(Context.GetOwner());
// 	
// 	if (!KarenActor || !InstanceData.PlayerActor)
// 	{
// 		return EStateTreeRunStatus::Failed;
// 	}
// 	
// 	FVector PlayerLocation = InstanceData.PlayerActor->GetActorLocation();
// 	FVector PlayerForward = InstanceData.PlayerActor->GetActorForwardVector();
// 	
// 	FVector FinalLocation = PlayerLocation;
// 	FRotator FinalRotation = KarenActor->GetActorRotation();
//
// 	
// 	switch (InstanceData.TeleportType)
// 	{
// 		case EKarenTeleportType::BehindPlayer: 
// 			FinalLocation = PlayerLocation - (PlayerForward * InstanceData.TeleportDistance);
// 			FinalRotation = PlayerForward.Rotation(); 
// 			break;
//
// 		case EKarenTeleportType::FrontOfPlayer: 
// 			FinalLocation = PlayerLocation + (PlayerForward * InstanceData.TeleportDistance);
// 			FinalRotation = (-PlayerForward).Rotation();
// 			break;
//
// 		case EKarenTeleportType::AbovePlayer: 
// 			FinalLocation = PlayerLocation + FVector(0.0f, 0.0f, InstanceData.TeleportHeight);
// 			
// 			FinalRotation = (PlayerLocation - FinalLocation).Rotation();
// 			break;
//
// 		case EKarenTeleportType::Retreat:
// 		{
// 			
// 			FVector DirectionAway = (KarenActor->GetActorLocation() - PlayerLocation).GetSafeNormal();
// 			DirectionAway.Z = 0.0f; 
// 			FinalLocation = PlayerLocation + (DirectionAway * (InstanceData.TeleportDistance + 400.0f));
// 			FinalRotation = (-DirectionAway).Rotation(); 
// 			break;
// 		}
//
// 		case EKarenTeleportType::Evade: 
// 		{
// 			
// 			FVector DirectionToPlayer = (PlayerLocation - KarenActor->GetActorLocation()).GetSafeNormal();
// 			DirectionToPlayer.Z = 0.0f; 
//
// 			
// 			FVector RightVector = FVector::CrossProduct(FVector::UpVector, DirectionToPlayer).GetSafeNormal();
//
// 			
// 			float EvadeDirection = FMath::RandBool() ? 1.0f : -1.0f;
// 			FVector SidestepVector = RightVector * EvadeDirection;
//
// 			
// 			FinalLocation = KarenActor->GetActorLocation() + (SidestepVector * InstanceData.TeleportDistance);
// 			FinalRotation = DirectionToPlayer.Rotation(); 
// 			break;
// 		}
// 	}
//
// 	
// 	KarenActor->SetActorLocation(FinalLocation);
// 	KarenActor->SetActorRotation(FinalRotation);
// 	
// 	return EStateTreeRunStatus::Succeeded;
// }