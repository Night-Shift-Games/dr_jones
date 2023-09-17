// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestData.h"
#include "Quest.generated.h"

class UQuestData;
class UQuest_OLD;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Category = "Quest System")
class DR_JONES_API UQuest_OLD : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FCompletedSignature, UQuest_OLD*);

	static UQuest_OLD* Construct(const UQuestData& QuestAsset);
	static UQuest_OLD* Construct(const FDataTableRowHandle& QuestRow);

	UQuest_OLD();

protected:
	virtual void OnAdded();
	virtual void OnRemoved();
	virtual void OnCompleted();

public:
	UFUNCTION(BlueprintCallable, Category = "Quest System")
	void MarkAsCompleted();

	bool IsCompleted() const;

	UFUNCTION(BlueprintPure, Category = "Quest System")
	const FName& GetQuestName() const;

	const UQuestData& GetQuestData() const;

	const FDataTableRowHandle& GetQuestTableRow() const;

	UFUNCTION(BlueprintPure, Category = "Quest System")
	const FQuestTableRow& GetQuestTableRowData() const;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest System", meta = (AllowPrivateAccess=true, ExposeOnSpawn=true))
	FDataTableRowHandle QuestTableRow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest System", meta = (AllowPrivateAccess=true))
	uint8 bIsCompleted : 1;

	friend class UQuestSystemLogic;
};

FORCEINLINE bool UQuest_OLD::IsCompleted() const
{
	return bIsCompleted;
}

FORCEINLINE const UQuestData& UQuest_OLD::GetQuestData() const
{
	check(QuestData);

	return *QuestData;
}

FORCEINLINE const FName& UQuest_OLD::GetQuestName() const
{
	check(QuestData);

	return QuestTableRow.RowName;
}

FORCEINLINE const FDataTableRowHandle& UQuest_OLD::GetQuestTableRow() const
{
	return QuestTableRow;
}

FORCEINLINE const FQuestTableRow& UQuest_OLD::GetQuestTableRowData() const
{
	return FQuestTableRow::GetQuestTableRow(QuestTableRow);
}
