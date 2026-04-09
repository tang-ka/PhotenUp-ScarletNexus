// FloatingPyramid.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPyramid.generated.h"

/**
 * 둥둥 떠다니는 사각뿔 오브젝트.
 * StaticMeshComponent 사용 — 블루프린트에서 메시를 교체 가능.
 * 기본값은 엔진 콘 메시.
 */
UCLASS()
class SCARLETNEXUS_API AFloatingPyramid : public AActor
{
	GENERATED_BODY()

public:
	AFloatingPyramid();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USceneComponent> SceneRoot;

	// 블루프린트에서 메시/머티리얼 자유롭게 교체 가능
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	// ── 부유 ──
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Float")
	float BobAmplitude = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Float")
	float BobSpeed = 2.f;

	// ── 회전 ──
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Float")
	float YawRotationSpeed = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Float")
	float TiltAmplitude = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Float")
	float TiltSpeed = 1.5f;

private:
	FVector OriginLocation;
	float RunningTime = 0.f;
};