// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(GT, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(SH, Warning, All)
DECLARE_LOG_CATEGORY_EXTERN(JJ, Log, All)

// --- 개별 로그 활성화 설정 (1: 켬, 0: 끔) ---
#define USE_LOG_GT 0
#define USE_LOG_SH 0
#define USE_LOG_JJ 0
// ------------------------------------------

// Shipping 빌드에서는 무조건 비활성화, 그 외 빌드에서는 개별 설정 참조
#if !UE_BUILD_SHIPPING
	#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

	// GT 로그 제어
	#if USE_LOG_GT
		#define PRINTLOG_GT(format, ...) UE_LOG(GT, Log, TEXT("%s %s"), *CALLINFO, *FString::Printf(format, ##__VA_ARGS__))
	#else
		#define PRINTLOG_GT(format, ...)
	#endif

	// SH 로그 제어
	#if USE_LOG_SH
		#define PRINTLOG_SH(format, ...) UE_LOG(SH, Warning, TEXT("%s %s"), *CALLINFO, *FString::Printf(format, ##__VA_ARGS__))
	#else
		#define PRINTLOG_SH(format, ...)
	#endif

	// JJ 로그 제어
	#if USE_LOG_JJ
		#define PRINTLOG_JJ(format, ...) UE_LOG(JJ, Log, TEXT("%s %s"), *CALLINFO, *FString::Printf(format, ##__VA_ARGS__))
	#else
		#define PRINTLOG_JJ(format, ...)
	#endif

#else
	// Shipping 빌드 전체 비활성화
	#define PRINTLOG_GT(format, ...)
	#define PRINTLOG_SH(format, ...)
	#define PRINTLOG_JJ(format, ...)
#endif