// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Item.h"

#include "JournalComponent.generated.h"

class ATool;
class UInputAction;
class UDrJonesWidgetBase;

USTRUCT(BlueprintType)
struct FJournalChapter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UDrJonesWidgetBase>> Pages;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumberOfPages = 0;
};

UENUM(BlueprintType)
enum class EJournalChapterType : uint8
{
	Tutorials,
	Quests,
	Log,
	Artifacts,
	Knowledge,
	
	NUM
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DR_JONES_API UJournalComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UJournalComponent();

	void SetupPlayerInputComponent(UEnhancedInputComponent& EnhancedInputComponent);

	void ConstructJournal();
	
	void OpenJournal();
	void SetActivePage(int NewPage);
	
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> OpenJournalAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AItem> JournalItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EJournalChapterType, FJournalChapter> Chapters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ATool> Journal;
	
	int ActiveLeftPage = 1;
};
