// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAttackCollision.generated.h"


UENUM(BlueprintType)
enum class EAttackCollisionShape : uint8
{
	Sphere,
	Box
};

/**
 * 적 공격 시 데미지 전달용 콜리전 컴포넌트
 * 
 * 사용법:
 *  1) 적 캐릭터에 이 컴포넌트를 추가
 *  2) 공격 시작 시 ActivateDamage() 호출 (AnimNotify 등에서)
 *  3) 공격 종료 시 DeactivateDamage() 호출
 *  4) 오버랩된 IDamageable 액터에 자동으로 데미지 전달
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UEnemyAttackCollision : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyAttackCollision();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	// ** 설정
	// 콜리전 모양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Collision")
	EAttackCollisionShape CollisionShape = EAttackCollisionShape::Sphere;
	
	// Sphere 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack Collision", meta = (EditCondition = "CollisionShape == EAttackCollisionShape::Sphere"))
	float SphereRadius = 80.f;
	
	// Box 반(半) Size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack Collision", meta = (EditCondition = "CollisionShape == EAttackCollisionShape::Box"))
	FVector BoxHalfExtent = FVector(60.f, 60.f, 60.f);
	
	// 소켓에 부착할 이름 (빈 문자열이면 루트에 부착)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack Collision")
	FName AttachSocketName = NAME_None;
	
	// 부착 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack Collision")
	FVector LocationOffset = FVector::ZeroVector;
	
	// 데미지 양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack Collision")
	int32 DamageAmount = 30;
	
	// 한 활성화 주기 내 같은 액터에 다시 데미지를 줄 수 있는지 체크
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack Collision")
	bool bAllowMultiHitPerActivation = false;
	
	// ** 기능
	// 콜리전 활성화 (공격 시작)
	UFUNCTION(BlueprintCallable, Category="Attack Collision")
	void ActivateCollision();
	
	// 콜리전 비활성화 (공격 종료)
	UFUNCTION(BlueprintCallable, Category="Attack Collision")
	void DeactivateCollision();
	
	// 현재 활성 상태 체크
	UFUNCTION(BlueprintPure, Category="Attack Collision")
	bool IsCollisionActive() const { return bIsActive; }
	
	// 히트 기록 초기화
	UFUNCTION(BlueprintCallable, Category="Attack Collision")
	void ClearHitActors();
	
	// 데미지 양 런타임 변경
	UFUNCTION(BlueprintCallable, Category="Attack Collision")
	void SetDamageAmount(int32 NewDamage) { DamageAmount = NewDamage; }
	
private:
	// 콜리전 Shape 콤포넌트 (런타임 설정)
	UPROPERTY()
	TObjectPtr<UShapeComponent> CollisionComp;
	
	// 활성 상태
	bool bIsActive = false;
	
	// 이번 활성화 주기 내 이미 히트된 액터 리스트
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActorList;
	
	// 콜리전 콤포넌트 생성 및 부착
	void CreateCollisionComponent();
	
	// 오버랩 콜백
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// 대상에게 데미지 적용 시도
	void TryApplyDamage(AActor* TargetActor);
};
