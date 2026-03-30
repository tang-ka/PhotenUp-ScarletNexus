// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerKasane.h"

#include "Player/Component/InputBufferComponent.h"


// Sets default values
APlayerKasane::APlayerKasane()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
		TEXT("'/Game/Models/GS_UModel/Characters/Kasane_GS/Kasane_SKM.Kasane_SKM'")
	);

	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetRelativeScale3D(FVector(0.42f, 0.42f, 0.42f));
	}
}

// Called when the game starts or when spawned
void APlayerKasane::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerKasane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerKasane::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerKasane::BasicAttack()
{
	Super::BasicAttack();
	
	// GetInputBufferComp()->BufferInput(EAttackType::BasicAttack);
}

void APlayerKasane::PsychicAttack()
{
	Super::PsychicAttack();
	
	// GetInputBufferComp()->BufferInput(EAttackType::PsychicAttack);
}

void APlayerKasane::BackStepAttack()
{
	Super::BackStepAttack();
	
	// GetInputBufferComp()->BufferInput(EAttackType::BackStepAttack);
}


