#pragma once

#include "CoreMinimal.h"
#include "AttackType.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None			UMETA(DisplayName = "None"),
	BasicAttack		UMETA(DisplayName = "Basic Attack"),
	PsychicAttack	UMETA(DisplayName = "Psychic Attack"),
	BackStepAttack	UMETA(DisplayName = "Back Step Attack")
};