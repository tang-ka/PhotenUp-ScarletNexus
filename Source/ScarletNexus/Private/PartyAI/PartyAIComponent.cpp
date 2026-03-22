// Fill out your copyright notice in the Description page of Project Settings.


#include "PartyAI/PartyAIComponent.h"

#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UPartyAIComponent::UPartyAIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPartyAIComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPartyAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPartyAIComponent::StartAI()
{
	if (!PartyCharacterST) return;

	StateTreeComp = Cast<UStateTreeComponent>(GetOwner()->AddComponentByClass(UStateTreeComponent::StaticClass(), false, FTransform::Identity, false));
	
	if (!StateTreeComp) return;

	StateTreeComp->SetStateTree(PartyCharacterST);
	StateTreeComp->StartLogic();
}
