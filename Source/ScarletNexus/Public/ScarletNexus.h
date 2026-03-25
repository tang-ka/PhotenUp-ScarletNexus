// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(SN, Log, All)

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
// 함수 호출 정보 매크로 함수
#define PRINT_CALLINFO() UE_LOG(SN, Log, TEXT("%s"), *CALLINFO)

// 가변 인자 받아서 출력
#define PRINTLOG(format, ...) UE_LOG(SN, Log, TEXT("%s %s"), *CALLINFO, *FString::Printf(format, ##__VA_ARGS__))