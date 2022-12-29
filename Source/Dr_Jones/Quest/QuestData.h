// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QuestData.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable, BlueprintType)
struct DR_JONES_API FQuestContentData
{
	GENERATED_BODY() 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest System")
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (MultiLine = "true"), Category = "Quest System")
	FText Content;
};

/**
 *
 */
USTRUCT(Blueprintable, BlueprintType)
struct DR_JONES_API FQuestNoteData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest System")
	class UStaticMesh* NoteMesh;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class DR_JONES_API UQuestData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest System")
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest System")
	FQuestContentData QuestContentData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest System")
	FQuestNoteData QuestNoteData;
};
