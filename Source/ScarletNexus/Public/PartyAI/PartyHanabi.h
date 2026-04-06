// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PartyAI/PartyMemberBase.h"
#include "PartyHanabi.generated.h"

class AWeaponSpear;
/**
 * 
 */
UCLASS()
class SCARLETNEXUS_API APartyHanabi : public APartyMemberBase
{
	GENERATED_BODY()
public:
	APartyHanabi();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	// 하나비 스탯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int HanabiATK = 35;

	// 무기 블루프린트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AWeaponSpear> WeaponClass;
	
	// 무기 소켓
	const FName WeaponSocketName = FName("Weapon");

	// 무기 부착 오프셋 (에디터에서 조정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FVector WeaponLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FRotator WeaponRotationOffset = FRotator::ZeroRotator;
	
	// 근거리 창 기본 공격
	UFUNCTION(BlueprintCallable, Category = Attack)
	void Attack();
	// 창 충돌 범위 (BoxTrace 크기)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Attack)
	FVector SpearBoxHalfExtent = FVector(150.f, 30.f, 30.f);
	// 기본 공격 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	float SpearAttackCooldown = 1.2f;
	
	// PK 콤포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK")
	TObjectPtr<class UPKComponent> PKComp;

	// 최종 데미지 계산용 랜덤 공격력
	UFUNCTION(BlueprintCallable)
	int GetHanabiATK();
	
	// 기본공격 : BoxTrace로 창 앞 범위 내 적 감지
	void PerformSpearTrace(TArray<FHitResult> hitResults);

	// 기본공격 충돌
	UFUNCTION()
	void OnSpearHit(AActor* HitActor);
};
