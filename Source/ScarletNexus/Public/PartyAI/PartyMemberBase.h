// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PartyMemberBase.generated.h"

class APlayerCharacterBase;
class USphereComponent;
/** CurrHP, MaxHP */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPartyMemberHPChangedDelegate, int32, int32);

UCLASS()
class SCARLETNEXUS_API APartyMemberBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APartyMemberBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AActor* WeaponActor;
	
	// 콤포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UPartyAIComponent> PartyAIComp;
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<class UStateTreeComponent> StateTreeComp;
	
	// 스탯
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	int MaxHP = 200;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	int CurrHP;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	int ATK = 20;
	
	UFUNCTION(BlueprintCallable, Category = "Status")
	virtual void TakeDamage_Party(int Damage);
	UFUNCTION(BlueprintCallable, Category = "Status")
	bool IsAlive() const { return CurrHP > 0; }
	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetHPPercent() const { return MaxHP > 0 ? static_cast<float>(CurrHP) / static_cast<float>(MaxHP) : 0.f; }

	/** HP가 변경될 때 브로드캐스트 (CurrHP, MaxHP) */
	FOnPartyMemberHPChangedDelegate OnHPChanged;
	
	// 쿨타임
	// 스킬 쿨타임 등록
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	void SetCooldown(FName SkillName, float Duration);
	// 스킬 사용 가능 여부 확인
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	bool IsSkillReady(FName SkillName) const;
	// 잔여 쿨타임
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	float GetRemainCooldown(FName SkillName) const;

	// 애니메이션 몽타주 재생 헬퍼
	UFUNCTION(BlueprintCallable, Category = "Animation")
	float PlayMontage(UAnimMontage* Montage, float PlayRate = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void StopMontage(UAnimMontage* Montage, float BlendOutTime = 0.25f);

	// 공격 및 스킬 데미지 전달
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ApplyDamageToHitTarget(AActor* HitTarget, AActor* Causer, int CauserATK);
	
	// 파티 AI 활성/비활성
	UFUNCTION(BlueprintCallable, Category = "Party")
	void ActivatePartyAI();
	
	UFUNCTION(BlueprintCallable, Category = "Party")
	void DeactivatePartyAI();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Party")
	bool bPartyActive = false;
	
	// ** 플레이어 상호작용
	// 상호 작용 범위 스피어
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	TObjectPtr<USphereComponent> InteractionSphere;
	
	// 플레이어가 상호작용 범위 안에 있는지 체크
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bPlayerInRange = false;
	
	// 플레이어가 말 걸었을 때 호출
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnInteract(APlayerCharacterBase* Requester);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> JoinPartySound;
	
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayJoinPartySound();
protected:
	// 스킬명 -> 쿨타임 종료 시각 (WorldTime)
	TMap<FName, float> CooldownEndTimes;
	
private:
	UFUNCTION()
	void OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
