// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Component/InputBufferComponent.h"

#include "ScarletNexus.h"


UInputBufferComponent::UInputBufferComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();

	InputBuffer.Reserve(MaxBufferSize);

	if (bEnableDebugLogs)
	{
		PRINTLOG_SH(TEXT("[InputBuffer] 컴포넌트 초기화 완료"));
	}
}

void UInputBufferComponent::BufferInput(EAttackType InType)
{
	if (InType == EAttackType::None)
	{
		PRINTLOG_SH(TEXT("[InputBuffer] None 타입 입력 무시"));
		return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();

	// 연타 방지: 같은 입력이 AntiSpamThreshold 초 이내에 들어왔는지 체크
	if (!InputBuffer.IsEmpty())
	{
		const FBufferedInput& LastInput = InputBuffer.Last();

		if (LastInput.AttackType == InType &&
			(CurrentTime - LastInput.Timestamp) < AntiSpamThreshold)
		{
			if (bEnableDebugLogs)
			{
				PRINTLOG_SH(TEXT("[InputBuffer] 연타 감지! 무시됨 (%.3f초 이내 중복)"),
				            CurrentTime - LastInput.Timestamp);
			}
			return;
		}
	}

	// 버퍼에 추가
	InputBuffer.Add({InType, CurrentTime});
	if (bEnableDebugLogs)
	{
		PRINTLOG_SH(TEXT("[InputBuffer] 입력 저장: %s | 시간: %.2f | 버퍼 크기: %d"),
			*GetAttackTypeName(InType), CurrentTime, InputBuffer.Num());
	}
}

bool UInputBufferComponent::ConsumeBufferedInput(EAttackType& OutAttackType)
{
	if (InputBuffer.IsEmpty())
	{
		return false;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();

	// 만료된 입력을 건너뛰고 유효한 첫 입력의 인덱스를 찾음
	int32 ValidIndex = INDEX_NONE;
	for (int32 i = 0; i < InputBuffer.Num(); ++i)
	{
		float InputAge = CurrentTime - InputBuffer[i].Timestamp;
		if (InputAge <= BufferDuration)
		{
			ValidIndex = i;
			break;
		}
	}

	if (ValidIndex == INDEX_NONE)
	{
		// 전부 만료 — 한 번에 비움
		ClearBuffer();
		return false;
	}

	OutAttackType = InputBuffer[ValidIndex].AttackType;

	// 유효 입력 포함 앞쪽 전부를 한 번에 제거
	InputBuffer.RemoveAt(0, ValidIndex + 1);
	return true;
}

void UInputBufferComponent::ClearBuffer()
{
	if (bEnableDebugLogs && !InputBuffer.IsEmpty())
	{
		PRINTLOG_SH(TEXT("[InputBuffer] 버퍼 클리어: %d개 입력 삭제"), InputBuffer.Num());
	}

	InputBuffer.Empty();
}

bool UInputBufferComponent::WasInputRecent(EAttackType InType, float WithinTime) const
{
	RemoveExpiredInputs();
	
	float CurrentTime = GetWorld()->GetTimeSeconds();

	for (int32 i = InputBuffer.Num() - 1; i >= 0; --i)
	{
		float InputAge = CurrentTime - InputBuffer[i].Timestamp;

		if (InputAge > AntiSpamThreshold)
		{
			break;
		}

		if (InputBuffer[i].AttackType == InType)
		{
			return true;
		}
	}

	return false;
}

void UInputBufferComponent::RemoveExpiredInputs() const
{
	if (InputBuffer.IsEmpty())
	{
		return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();
	int32 FirstValidIndex = 0;

	while (FirstValidIndex < InputBuffer.Num() &&
		   (CurrentTime - InputBuffer[FirstValidIndex].Timestamp) > BufferDuration)
	{
		++FirstValidIndex;
	}

	if (FirstValidIndex > 0)
	{
		if (bEnableDebugLogs)
		{
			PRINTLOG_SH(TEXT("[InputBuffer] 만료된 입력 제거: %d개 제거"), FirstValidIndex);
		}

		InputBuffer.RemoveAt(0, FirstValidIndex);
	}
}

FString UInputBufferComponent::GetAttackTypeName(EAttackType Type) const
{
	const UEnum* EnumPtr = StaticEnum<EAttackType>();
	if (EnumPtr)
	{
		return EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Type)).ToString();
	}
	return FString::Printf(TEXT("%d"), static_cast<int32>(Type));
}
