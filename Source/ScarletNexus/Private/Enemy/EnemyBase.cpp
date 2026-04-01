// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyBase.h"

#include "Misc/MapErrors.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::OnAttack()
{
}

void AEnemyBase::OnHit()
{
}

void AEnemyBase::Stun(float Duration)
{
}

void AEnemyBase::OnDie()
{
	// 죽었을 때 Anim Ragdoll 처리, Destroy 연출
	Destroy();
}

bool AEnemyBase::ReceiveDamage_Implementation(FDamageInfo DamageInfo)
{
	return IDamageable::ReceiveDamage_Implementation(DamageInfo);
}

int AEnemyBase::GetHP_Implementation() const
{
	return IDamageable::GetHP_Implementation();
}

float AEnemyBase::GetHPPercent_Implementation() const
{
	return IDamageable::GetHPPercent_Implementation();
}

bool AEnemyBase::IsDead_Implementation() const
{
	return IDamageable::IsDead_Implementation();
}
