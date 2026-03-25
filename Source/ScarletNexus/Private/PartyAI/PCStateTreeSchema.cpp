// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PCStateTreeSchema.h"
#include "StateTreeConditionBase.h"
#include "StateTreeTaskBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"
#include "PartyAI/PartyAIComponent.h"
#include "PartyAI/PartyCharacterStateContext.h"

bool UPCStateTreeSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	// Task, Condition, Evaluator 구조체 허용
	return InScriptStruct->IsChildOf(FStateTreeTaskBase::StaticStruct())
		|| InScriptStruct->IsChildOf(FStateTreeConditionBase::StaticStruct())
		|| InScriptStruct->IsChildOf(FStateTreeEvaluatorBase::StaticStruct());
}

bool UPCStateTreeSchema::IsClassAllowed(const UClass* InClass) const
{
	// UStateTreeComponent를 소유 콤포넌트로 지정
	return InClass->IsChildOf(UStateTreeComponent::StaticClass());
}

bool UPCStateTreeSchema::IsExternalItemAllowed(const UStruct& InStruct) const
{
	// FPartyCharacterStateContext를 외부 데이터로 허용
	return InStruct.IsChildOf(FPartyCharacterStateContext::StaticStruct());
}
