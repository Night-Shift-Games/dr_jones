// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Quest.generated.h"

class UQuestData;
class UQuest;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Category = "Quest System")
class DR_JONES_API UQuest : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FCompletedSignature, UQuest*);

	static UQuest* Construct(const UQuestData& QuestAsset);

	UQuest();

	virtual void OnAdded();
	virtual void OnRemoved();
	virtual void OnCompleted();

	const UQuestData& GetQuestData() const;

	FCompletedSignature CompletedDelegate;

	// Blueprint interface

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest System", meta = (DisplayName = "On Added"))
	void K2_OnAdded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest System", meta = (DisplayName = "On Removed"))
	void K2_OnRemoved();

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest System", meta = (DisplayName = "On Completed"))
	void K2_OnCompleted();

	// End of Blueprint interface

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest System", meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	TObjectPtr<const UQuestData> QuestData;
};

FORCEINLINE const UQuestData& UQuest::GetQuestData() const
{
	check(QuestData);

	return *QuestData;
}
