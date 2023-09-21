// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArchaeologicalSite/ArchaeologicalSite.h"
#include "GameFramework/GameModeBase.h"
#include "Player/DrJonesCharacter.h"

#include "Dr_JonesGameModeBase.generated.h"

class AIlluminati;
class UQuestSystemLogic;
class UWorldData;

DECLARE_DYNAMIC_DELEGATE(FQuestSystemInitializedDynamicDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQuestSystemInitializedMCDelegate);

UCLASS()
class DR_JONES_API ADr_JonesGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADr_JonesGameModeBase();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UQuestSystemLogic* GetQuestSystem() const;

	UFUNCTION(BlueprintCallable)
	AArchaeologicalSite* FindOrCreateArchaeologicalSite();
	
	/** Call an event only when the quest system gets loaded, or immediately, if it already is. */
	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void ExecutePostQuestSystemLoad(UPARAM(DisplayName="Event") const FQuestSystemInitializedDynamicDelegate& Delegate);

	UFUNCTION(BlueprintPure)
	UWorldData* GetGlobalWorldData() const;

	UFUNCTION(BlueprintPure)
	AIlluminati* GetIlluminati() const;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UWorldData> GlobalWorldData;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AArchaeologicalSite> ArchaeologicalSite;

	// X-Files theme starts playing
	UPROPERTY(SaveGame, Instanced, BlueprintReadOnly)
	TObjectPtr<AIlluminati> Illuminati;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AIlluminati> IlluminatiClass;

private:
	FQuestSystemInitializedMCDelegate OnQuestSystemInitializedDelegate;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest System", meta = (AllowPrivateAccess = true))
	TObjectPtr<UQuestSystemLogic> QuestSystem;
};

FORCEINLINE AArchaeologicalSite* ADr_JonesGameModeBase::FindOrCreateArchaeologicalSite()
{
	if (!ArchaeologicalSite)
	{
		ArchaeologicalSite = GetWorld()->SpawnActor<AArchaeologicalSite>();
	}
	return ArchaeologicalSite;
}

FORCEINLINE UQuestSystemLogic* ADr_JonesGameModeBase::GetQuestSystem() const
{
	return QuestSystem;
}

FORCEINLINE UWorldData* ADr_JonesGameModeBase::GetGlobalWorldData() const
{
	return GlobalWorldData;
}

FORCEINLINE AIlluminati* ADr_JonesGameModeBase::GetIlluminati() const
{
	return Illuminati;
}