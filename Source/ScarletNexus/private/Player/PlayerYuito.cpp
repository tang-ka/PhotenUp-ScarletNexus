// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerYuito.h"


// Sets default values
APlayerYuito::APlayerYuito()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
		TEXT("'/Game/Models/Yuito/Yuito.Yuito'")
	);

	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetRelativeScale3D(FVector(1.55f, 1.55f, 1.55f));
	}
}

// Called when the game starts or when spawned
void APlayerYuito::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerYuito::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerYuito::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
