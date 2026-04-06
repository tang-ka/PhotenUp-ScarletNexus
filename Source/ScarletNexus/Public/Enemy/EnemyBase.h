// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/Damageable.h"
#include "EnemyBase.generated.h"

class UDissolveComponent;
class UBossOverheadWidget;
class UWidgetComponent;

UCLASS()
class SCARLETNEXUS_API AEnemyBase : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 스테이터스
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Status")
	int MaxHP = 250;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CurrHP = MaxHP;
	
	// 타겟
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> TargetActor;
	
	// **히트 리액션
	// 경직 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	float HitStunDuration = 0.3f;
	
	// 현재 경직 상태인지 체크
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Hit")
	bool bIsStunned = false;
	
	// 블루프린트에서 히트 몽타주 할당
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Hit")
	TObjectPtr<UAnimMontage> HitReactionMontage;
	
	//**사망
	// 사망 후 Destroy까지 딜레이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Die")
	float DestroyDelay = 5.f;
	
	// Ragdoll 활성화 체크
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Die")
	bool bEnableRagdollOnDeath = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Die")
	bool bIsDie = false;
	
	//**체력 바 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<UWidgetComponent> HealthBarComp;
	
	// 블루프린트 위젯 클래스 할당
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> HPBarWidgetClass;
	
	UFUNCTION(BlueprintCallable, Category="UI")
	void UpdateHealthBar();
	
	UFUNCTION(BlueprintCallable, Category="Status")
	bool IsAlive() const { return CurrHP > 0; }
	
	//**AI
	// StateTree 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<class UStateTree> EnemyStateTree;
	
	// 소속 EnemyManager
	UPROPERTY()
	TObjectPtr<class AEnemyManager> OwningManager;
	
	// HP 비율 (0~1) : 체력바 위젯 바인딩용
	UFUNCTION(BlueprintCallable, Category="Status")
	float GetHPRatio() const;
	
	// 사망 처리 (ST Die Task에서 호출)
	UFUNCTION(BlueprintCallable, Category="Die")
	virtual void Die();
	
	//**FX
	// Dissolve
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FX")
	UDissolveComponent* DissolveComp;
	
#pragma region IDamageable
	bool ReceiveDamage_Implementation(FDamageInfo DamageInfo) override;
	int GetHP_Implementation() const override;
	float GetHPPercent_Implementation() const override;
	bool IsDead_Implementation() const override;
#pragma endregion 
	
protected:
	// Ragdoll 전환
	void EnableRagdoll();
	
private:
	FTimerHandle DestroyTimerHandle;
};
