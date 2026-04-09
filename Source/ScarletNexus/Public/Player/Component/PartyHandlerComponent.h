// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PartyHandlerComponent.generated.h"

class APartyMemberBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPartyMemberAddedDelegate, APartyMemberBase*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPartyMemberRemovedDelegate, APartyMemberBase*);

/**
 * 파티 캐릭터를 관리하는 컴포넌트.
 * PlayerCharacterBase에 부착되며 최대 MaxPartySize(1)명의 파티원을 풀로 관리합니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARLETNEXUS_API UPartyHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPartyHandlerComponent();

	/** 최대 파티 인원 */
	static constexpr int32 MaxPartySize = 1;

#pragma region Party Management
	/**
	 * 파티원을 추가합니다.
	 * @return 성공 시 true (풀이 가득 찼거나 이미 파티원이면 false)
	 */
	UFUNCTION(BlueprintCallable, Category = "Party")
	bool AddPartyMember(APartyMemberBase* Member);

	/**
	 * 파티원을 제거합니다.
	 * @return 성공 시 true
	 */
	UFUNCTION(BlueprintCallable, Category = "Party")
	bool RemovePartyMember(APartyMemberBase* Member);

	/** 인덱스로 파티원을 가져옵니다. */
	UFUNCTION(BlueprintCallable, Category = "Party")
	APartyMemberBase* GetPartyMember(int32 Index) const;

	/** 현재 파티 인원 수 */
	UFUNCTION(BlueprintCallable, Category = "Party")
	int32 GetPartySize() const { return PartyPool.Num(); }

	/** 파티가 가득 찼는지 여부 */
	UFUNCTION(BlueprintCallable, Category = "Party")
	bool IsFull() const { return PartyPool.Num() >= MaxPartySize; }

	/** 해당 캐릭터가 파티원인지 확인 */
	UFUNCTION(BlueprintCallable, Category = "Party")
	bool IsInParty(APartyMemberBase* Member) const;

	/** 파티 전체 해제 */
	UFUNCTION(BlueprintCallable, Category = "Party")
	void ClearParty();

	/** 파티 풀 전체 반환 */
	const TArray<TObjectPtr<APartyMemberBase>>& GetPartyPool() const { return PartyPool; }
#pragma endregion

public:
	/** 파티원이 추가되었을 때 브로드캐스트 */
	FOnPartyMemberAddedDelegate OnPartyMemberAdded;
	/** 파티원이 제거되었을 때 브로드캐스트 */
	FOnPartyMemberRemovedDelegate OnPartyMemberRemoved;

private:
	UPROPERTY(VisibleAnywhere, Category = "Party")
	TArray<TObjectPtr<APartyMemberBase>> PartyPool;
};

