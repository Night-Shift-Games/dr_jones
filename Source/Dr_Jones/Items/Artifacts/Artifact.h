// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"

#include "Artifact.generated.h"

UENUM()
enum class EArtifactRarity
{
	Common,
	Uncommon,
	Lore,
	Legendary,
	Quest
};

UENUM()
enum class EArtifactSize
{
	Tiny,
	Small,
	Medium,
	Large,
	Huge
};


UCLASS(Blueprintable)
class DR_JONES_API AArtifact : public AItem
{
	GENERATED_BODY()

public:
	
	virtual void Interact(APawn* Indicator);

	void Take(APawn* Taker);
	
	UPROPERTY(EditAnywhere)
	FString InteractionSentence;

	void SetParameters(AArtifact* Other);

	UFUNCTION(BlueprintImplementableEvent)
	void OnArtifactTake(APawn* Taker);

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Item", meta = (DisplayPriority = 4))
	EArtifactRarity Rarity;

	UPROPERTY(EditAnywhere, Category = "Item", meta = (DisplayPriority = 5))
	EArtifactSize Size;
};

