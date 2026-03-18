// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/PlayerPerceptionComponent.h"


UPlayerPerceptionComponent::UPlayerPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPlayerPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UPlayerPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

