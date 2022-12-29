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
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestCompletedSignature, UQuest*, Quest);

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void AddQuest(UQuest* NewQuest);

	UFUNCTION(BlueprintPure, Category = "Quest System")
	bool HasReceivedQuest(const FName& QuestName) const;

	UFUNCTION(BlueprintCallable, Category = "Quest System")
	TArray<UQuest*> GetQuestsArray() const;

	const TMap<FName, TObjectPtr<UQuest>>& GetQuestsMap() const;

	/**
	 * Called when a new quest gets added by the AddQuest function.
	 */
	virtual void OnQuestReceived(UQuest* Quest);

	UPROPERTY(BlueprintAssignable, Category = "Quest System", meta = (DisplayName = "On Quest Received", ScriptName = "On Quest Received"))
	FQuestReceivedSignature QuestReceivedDelegate;

	virtual void OnQuestCompleted(UQuest* Quest);

	UPROPERTY(BlueprintAssignable, Category = "Quest System", meta = (DisplayName = "On Quest Completed", ScriptName = "On Quest Completed"))
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
