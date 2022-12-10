// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Item.h"
#include "../../Interactive/InteractiveObject.h"

#include "Artefact.generated.h"

UENUM()
enum FArtefactRarity
{
	Common,
	Uncommon,
	Lore,
	Legendary,
	Quest
};

UENUM()
enum FArtefactSize
{
	Tiny,
	Small,
	Medium,
	Large,
	Huge
};


UCLASS(Blueprintable)
class DR_JONES_API UArtefact : public UItem, public IInteractiveObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category="Item", meta = (MultiLine = true, DisplayPriority = 3))
	FString Description;

	UPROPERTY(EditAnywhere, Category = "Item", meta = (DisplayPriority = 4))
	TEnumAsByte<FArtefactRarity> Rarity;

	UPROPERTY(EditAnywhere, Category = "Item", meta = (DisplayPriority = 5))
	TEnumAsByte<FArtefactSize> Size;

	virtual void BeginPlay() override;
	
	virtual void Interact(APawn* Indicator) override;

	void Take(APawn* Taker);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnArtefactTake(APawn* Taker);
};

