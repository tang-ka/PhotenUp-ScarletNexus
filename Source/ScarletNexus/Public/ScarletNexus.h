// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(GT, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(SH, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(JJ, Log, All)

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

// 로그 매크로
#define PRINTLOG_GT(format, ...) UE_LOG(GT, Log, TEXT("%s %s"), *CALLINFO, *FString::Printf(format, ##__VA_ARGS__))
#define PRINTLOG_SH(format, ...) UE_LOG(SH, Log, TEXT("%s %s"), *CALLINFO, *FString::Printf(format, ##__VA_ARGS__))
#define PRINTLOG_JJ(format, ...) UE_LOG(SH, Log, TEXT("%s %s"), *CALLINFO, *FString::Printf(format, ##__VA_ARGS__))