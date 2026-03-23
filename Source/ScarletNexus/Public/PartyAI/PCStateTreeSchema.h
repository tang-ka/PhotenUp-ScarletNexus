// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeSchema.h"
#include "PCStateTreeSchema.generated.h"

class UActorComponent;
/**
 * 
 */
UCLASS(EditInlineNew, CollapseCategories, meta=(DisplayName="Party Character ST", CommonSchema))
class SCARLETNEXUS_API UPCStateTreeSchema : public UStateTreeSchema
{
	GENERATED_BODY()

protected:
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override;
	virtual bool IsClassAllowed(const UClass* InScriptStruct) const override;
	virtual bool IsExternalItemAllowed(const UStruct& InStruct) const override;
};
