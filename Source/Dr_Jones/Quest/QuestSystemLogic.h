// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestSystemLogic.generated.h"

class UQuest;
class UQuestData;

/**
 * 
 */
UCLASS(BlueprintType, NotBlueprintable, Category = "Quest System")
class DR_JONES_API UQuestSystemLogic : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestReceivedSignature, UQuest*, Quest);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestRemovedSignature, UQuest*, Quest);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestCompletedSignature, UQuest*, Quest);

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void AddQuest(UQuest* NewQuest);

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void RemoveQuest(const FName& QuestName);

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	UQuest* RemoveAndRetrieveQuest(const FName& QuestName);

	UQuest* FindQuest(const FName& QuestName);

	UFUNCTION(BlueprintPure, Category = "Quest System", meta=(DisplayName="Find Quest"))
	void K2_FindQuest(
		const FName& QuestName,
		UPARAM(DisplayName = "Found") bool& bOutFound,
		UPARAM(DisplayName = "Quest") UQuest*& OutQuest
	);

	UFUNCTION(BlueprintPure, Category = "Quest System")
	bool HasReceivedQuest(const FName& QuestName) const;

	UFUNCTION(BlueprintPure, Category = "Quest System")
	TArray<UQuest*> GetQuestsArray() const;

	const TMap<FName, TObjectPtr<UQuest>>& GetQuestsMap() const;

	/** Called when a new quest gets added by the AddQuest function. */
	virtual void OnQuestReceived(UQuest* Quest);

	UPROPERTY(BlueprintAssignable, Category = "Quest System", meta = (DisplayName = "On Quest Received"))
	FQuestReceivedSignature QuestReceivedDelegate;

	/** Called when a quest gets removed by any of the RemoveQuest functions. */
	virtual void OnQuestRemoved(UQuest* Quest);

	UPROPERTY(BlueprintAssignable, Category = "Quest System", meta = (DisplayName = "On Quest Removed"))
	FQuestRemovedSignature QuestRemovedDelegate;

	/** Called when a quest is completed (externally). */
	virtual void OnQuestCompleted(UQuest* Quest);

	UPROPERTY(BlueprintAssignable, Category = "Quest System", meta = (DisplayName = "On Quest Completed"))
	FQuestCompletedSignature QuestCompletedDelegate;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest System", meta=(AllowPrivateAccess=true))
	TMap<FName, TObjectPtr<UQuest>> Quests;
};

FORCEINLINE bool UQuestSystemLogic::HasReceivedQuest(const FName& QuestName) const
{
	return Quests.Contains(QuestName);
}

FORCEINLINE TArray<UQuest*> UQuestSystemLogic::GetQuestsArray() const
{
	TArray<TObjectPtr<UQuest>> QuestsArray;
	QuestsArray.Reserve(Quests.Num());

	Quests.GenerateValueArray(QuestsArray);
	return QuestsArray;
}

FORCEINLINE const TMap<FName, TObjectPtr<UQuest>>& UQuestSystemLogic::GetQuestsMap() const
{
	return Quests;
}
