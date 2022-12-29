// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Quest.generated.h"

class UQuestData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQuestAddedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQuestRemovedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FQuestCompletedSignature);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Category = "Quest System")
class DR_JONES_API UQuest : public UObject
{
	GENERATED_BODY()

public:
	static UQuest* Construct(const UQuestData& QuestAsset);

	UQuest();

	virtual void OnAdded();
	virtual void OnRemoved();
	virtual void OnCompleted();

	const UQuestData& GetQuestData() const;

	// Blueprint interface

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest System", meta = (DisplayName = "On Added"))
	void K2_OnAdded();

	UPROPERTY(BlueprintAssignable, Category = "Quest System", meta = (DisplayName = "On Quest Added"))
	FQuestAddedSignature QuestAddedDelegate;

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest System", meta = (DisplayName = "On Removed"))
	void K2_OnRemoved();

	UPROPERTY(BlueprintAssignable, Category = "Quest System", meta = (DisplayName = "On Quest Removed"))
	FQuestRemovedSignature QuestRemovedDelegate;

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest System", meta = (DisplayName = "On Completed"))
	void K2_OnCompleted();

	UPROPERTY(BlueprintAssignable, Category = "Quest System", meta = (DisplayName = "On Quest Completed"))
	FQuestCompletedSignature QuestCompletedDelegate;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest System", meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	TObjectPtr<const UQuestData> QuestData;
};

FORCEINLINE const UQuestData& UQuest::GetQuestData() const
{
	return *QuestData;
}
