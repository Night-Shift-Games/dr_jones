// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestSystemLogic.generated.h"

class UQuestData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestReceivedSignature, UQuestData*, Quest);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Category = "Quest System")
class DR_JONES_API UQuestSystemLogic : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void AddQuest(UQuestData* NewQuest);

	UFUNCTION(BlueprintPure, Category = "Quest System")
	bool HasReceivedQuest(UQuestData* ExistingQuest) const;

	UFUNCTION(BlueprintPure, Category = "Quest System")
	const TArray<UQuestData*>& GetQuests() const;

	/**
	 * Called when a new quest gets added by the AddQuest function.
	 */
	virtual void OnQuestReceived(UQuestData* Quest);

	UPROPERTY(BlueprintAssignable, Category = "Quest System", meta = (DisplayName = "On Quest Received", ScriptName = "On Quest Received"))
	FQuestReceivedSignature QuestReceivedDelegate;

	/**
	 * Called when a new quest gets added by the AddQuest function.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Quest System", meta = (DisplayName = "On Quest Received", ScriptName = "On Quest Received"))
	void K2_OnQuestReceived(UQuestData* Quest);

private:
	UPROPERTY(VisibleAnywhere, Category = "Quest System")
	TArray<TObjectPtr<UQuestData>> Quests;
};

FORCEINLINE void UQuestSystemLogic::AddQuest(UQuestData* NewQuest)
{
	check(NewQuest);

	Quests.Add(NewQuest);

	OnQuestReceived(NewQuest);
}

FORCEINLINE bool UQuestSystemLogic::HasReceivedQuest(UQuestData* ExistingQuest) const
{
	return Quests.Contains(ExistingQuest);
}

FORCEINLINE const TArray<UQuestData*>& UQuestSystemLogic::GetQuests() const
{
	return Quests;
}
