// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacterBase.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/RotationMatrix.h"

APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
		TEXT("/Game/SSH/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple")
	);

	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
	SpringArmComp->TargetArmLength = 500.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	
	bUseControllerRotationYaw = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Add the input mapping context
	auto* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(IMC_Player, 0);
		}
	}
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	auto* InputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (InputComp)
	{
		// Movement
		InputComp->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Move);
		InputComp->BindAction(IA_Look, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Look);
		InputComp->BindAction(IA_Jump, ETriggerEvent::Started, this, &APlayerCharacterBase::Jump);
		InputComp->BindAction(IA_Jump, ETriggerEvent::Completed, this, &APlayerCharacterBase::StopJumpingAction);
		InputComp->BindAction(IA_Dodge, ETriggerEvent::Started, this, &APlayerCharacterBase::Dodge);

		// Skills
		InputComp->BindAction(IA_BasicAttack, ETriggerEvent::Started, this, &APlayerCharacterBase::BasicAttack);
		InputComp->BindAction(IA_Psychokinesis, ETriggerEvent::Started, this, &APlayerCharacterBase::Psychokinesis);
	}
}

void APlayerCharacterBase::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller == nullptr)
	{
		return;
	}

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	const FVector FinalDirection = ForwardDirection * MovementVector.X + RightDirection * MovementVector.Y;
	
	AddMovementInput(FinalDirection);
}

void APlayerCharacterBase::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(-LookVector.Y);
}

void APlayerCharacterBase::Jump(const FInputActionValue& Value)
{
	ACharacter::Jump();
}

void APlayerCharacterBase::StopJumpingAction(const FInputActionValue& Value)
{
	StopJumping();
}

void APlayerCharacterBase::Dodge(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge!"));
}

void APlayerCharacterBase::BasicAttack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Basic Attack!"));
}

void APlayerCharacterBase::Psychokinesis(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Psychokinesis!"));
}

