// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/DrJonesCharacter.h"

#include "Dr_JonesGameModeBase.generated.h"

class UQuestSystemLogic;

DECLARE_DYNAMIC_DELEGATE(FQuestSystemInitializedDynamicDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQuestSystemInitializedMCDelegate);

UCLASS()
class DR_JONES_API ADr_JonesGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	ADr_JonesGameModeBase();

	virtual void BeginPlay() override;

	UQuestSystemLogic* GetQuestSystem() const;

	/** Call an event only when the quest system gets loaded, or immediately, if it already is. */
	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void ExecutePostQuestSystemLoad(UPARAM(DisplayName="Event") const FQuestSystemInitializedDynamicDelegate& Delegate);

private:
	FQuestSystemInitializedMCDelegate OnQuestSystemInitializedDelegate;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest System", meta=(AllowPrivateAccess=true))
	TObjectPtr<UQuestSystemLogic> QuestSystem;
};

FORCEINLINE UQuestSystemLogic* ADr_JonesGameModeBase::GetQuestSystem() const
{
	return QuestSystem;
}
