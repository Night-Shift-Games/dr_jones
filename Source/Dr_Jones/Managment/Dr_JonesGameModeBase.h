// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Items/Artifacts/ArtifactDatabase.h"

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

	virtual void InitGameState() override;
	virtual void Tick(float DeltaSeconds) override;

	UQuestSystemLogic* GetQuestSystem() const { return QuestSystem; }
	
	/** Call an event only when the quest system gets loaded, or immediately, if it already is. */
	UFUNCTION(BlueprintCallable, Category = "DrJones|Quest System")
	void ExecutePostQuestSystemLoad(UPARAM(DisplayName="Event") const FQuestSystemInitializedDynamicDelegate& Delegate);

	UFUNCTION(BlueprintPure)
	UWorldData* GetGlobalWorldData() const { return GlobalWorldData; }

	UFUNCTION(BlueprintPure)
	AIlluminati* GetIlluminati() const { return Illuminati; }

	UFUNCTION(BlueprintPure)
	UArtifactDatabase* GetArtifactDataBase() const { return ArtifactDatabase; }
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UWorldData> GlobalWorldData;

	// X-Files theme starts playing
	UPROPERTY(SaveGame, Instanced, BlueprintReadOnly)
	TObjectPtr<AIlluminati> Illuminati;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AIlluminati> IlluminatiClass;

	// NOTE: Temp solution to get rid of BP based artifacts. In long term we need find better place for it. Settings?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UArtifactDatabase> ArtifactDatabase;
	
private:
	FQuestSystemInitializedMCDelegate OnQuestSystemInitializedDelegate;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DrJones|Quest System", meta = (AllowPrivateAccess = true))
	TObjectPtr<UQuestSystemLogic> QuestSystem;
};