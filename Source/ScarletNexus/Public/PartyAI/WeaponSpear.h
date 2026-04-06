// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSpear.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class SCARLETNEXUS_API AWeaponSpear : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponSpear();

	// 콜리전 활성/비활성 (공격 타이밍에 호출)
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void EnableAttackCollision();
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void DisableAttackCollision();

	// 불꽃 FX 활성/비활성
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void EnableFireFX();
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void DisableFireFX();

	// Hit 발생 이벤트 (캐릭터에서 바인딩)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpearHit, AActor*, HitActor);
	UPROPERTY(BlueprintAssignable, Category="Weapon")
	FOnSpearHit OnSpearHit;

	// 창 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> SpearMesh;

	// 창 끝 콜리전 (SphereCollision)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<class USphereComponent> TipCollision;

	// 불꽃 Niagara FX
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UNiagaraComponent> FireFX;

	// 에디터에서 FX 에셋 지정
	UPROPERTY(EditAnywhere, Category="Weapon")
	TObjectPtr<UNiagaraSystem> FireFXAsset;

	// 이미 히트한 액터 중복 방지
	TArray<TObjectPtr<AActor>> HitActors;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnTipOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
