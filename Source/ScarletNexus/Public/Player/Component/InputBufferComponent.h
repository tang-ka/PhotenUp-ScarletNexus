// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ComboAttackDataAsset.h"
#include "InputBufferComponent.generated.h"

USTRUCT(BlueprintType)
struct FBufferedInput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType AttackType = EAttackType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Timestamp = 0.f;

	FBufferedInput()
	{
	}

	FBufferedInput(EAttackType InType, float InTimestamp)
		: AttackType(InType), Timestamp(InTimestamp)
	{
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInputBufferComponent();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void BufferInput(EAttackType InType);

	UFUNCTION()
	bool ConsumeBufferedInput(EAttackType& OutAttackType);

	UFUNCTION()
	void ClearBuffer();

	UFUNCTION()
	bool WasInputRecent(EAttackType InType, float WithinTime) const;

	UFUNCTION()
	bool HasBufferedInput() const
	{
		RemoveExpiredInputs();
		return InputBuffer.Num() > 0;
	}

	UFUNCTION()
	int32 GetBufferedInputCount() const
	{
		RemoveExpiredInputs();
		return InputBuffer.Num();
	}

private:
	// 오래된 입력 제거 (Tick에서 호출)
	void RemoveExpiredInputs() const;
	FString GetAttackTypeName(EAttackType Type) const;

private:
	UPROPERTY(VisibleAnywhere, Category = "Input Buffer")
	mutable TArray<FBufferedInput> InputBuffer;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input Buffer", meta = (ClampMin = "1", ClampMax = "20"))
	int32 MaxBufferSize = 5;
	
	// 버퍼 유지 시간 (이보다 오래된 입력은 자동 삭제)
	UPROPERTY(EditAnywhere, Category = "Input Buffer", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float BufferDuration = 1.0f;
    
	// 연타 방지 시간 (같은 입력이 이 시간 내에 들어오면 무시)
	UPROPERTY(EditAnywhere, Category = "Input Buffer", meta = (ClampMin = "0.0", ClampMax = "0.5"))
	float AntiSpamThreshold = 0.1f;
	
	// 디버그 로그 활성화
	UPROPERTY(EditAnywhere, Category = "Input Buffer")
	bool bEnableDebugLogs = true;
	
	FTimerHandle BufferCleanupTimer;
};
