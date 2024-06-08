// Property of Night Shift Games, all rights reserved.

#pragma once

#include "Items/Item.h"

#include "Letter.generated.h"

class UWidgetComponent;

UCLASS()
class DR_JONES_API ALetter : public AItem
{
	GENERATED_BODY()

public:
	ALetter();

	virtual void OnEquip() override;
	virtual void OnUnequip() override;
	
public:
	// Content of the letter.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DrJones", meta = (MultiLine))
	FText LetterContent = FText::AsCultureInvariant(TEXT("None"));

	// Display mesh of an letter.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones")
	TObjectPtr<UStaticMeshComponent> LetterMeshComponent;
};
