// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPyramid.generated.h"

UCLASS()
class SCARLETNEXUS_API AFloatingPyramid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPyramid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ── 콤포넌트 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USceneComponent> SceneRoot;
	
	// ** 메쉬 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	TObjectPtr<class UProceduralMeshComponent> PyramidMesh;
	
	// 피라미드 밑변 한 변의 길이의 절반
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pyramid|Shape")
	float BaseHalfSize = 50.f;
	
	// 피라미드 높이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pyramid|Shape")
	float PyramidHeight = 120.f;
	
	// ** 부유
	// 상하 진폭 (cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pyramid|Float")
	float BobAmplitude = 30.f;
	
	// 상하 주기 속도 (rad/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pyramid|Float")
	float BobSpeed = 2.f;
	
	// ** 회전
	// Yaw 회전 속도 (deg/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pyramid|Float")
	float YawRotationSpeed = 45.f;
	
	// Pitch 흔들림 진폭 (deg)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pyramid|Float")
	float TiltAmplitude = 8.f;
 
	// Pitch 흔들림 속도 (rad/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pyramid|Float")
	float TiltSpeed = 1.5f;
 
	// ── 머티리얼 ──
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pyramid|Visual")
	TObjectPtr<UMaterialInterface> PyramidMaterial;
	
private:
	void BuildPyramidMesh();
 
	// 부유 기준 위치 (BeginPlay 시점 위치)
	FVector OriginLocation;
	float RunningTime = 0.f;
};
