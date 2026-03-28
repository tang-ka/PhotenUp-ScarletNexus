// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/ComboComponent.h"


UComboComponent::UComboComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UComboComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UComboComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

