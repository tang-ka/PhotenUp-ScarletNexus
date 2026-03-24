// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PartyAI/PartyMemberBase.h"
#include "PartyHanabi.generated.h"

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
	
	// 근거리 창 기본 공격
	UFUNCTION(BlueprintCallable, Category = Attack)
	void Attack();
	// 창 충돌 범위 (BoxTrace 크기)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Attack)
	FVector SpearBoxHalfExtent = FVector(150.f, 30.f, 30.f);
	// 기본 공격 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	float SpearAttackCooldown = 1.2f;
	
	// 염력 스킬 (집기 -> 던지기)
	UFUNCTION(BlueprintCallable, Category = PK)
	void SkillPK();
	// 염력으로 집을 수 있는 최대 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PK)
	float PKRange = 800.f;
	// 염력 던지는 힘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PK")
	float PKForce = 2000.f;
	// 염력 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PK")
	float PKCooldown = 3.f;
	
	// PK 콤포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK")
	TObjectPtr<class UPKComponent> PKComp;

private:
	// 기본공격 : BoxTrace로 창 앞 범위 내 적 감지
	void PerformSpearTrace(TArray<FHitResult> hitResults);
};
