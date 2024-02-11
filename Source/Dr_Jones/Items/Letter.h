// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Items/Item.h"

#include "Letter.generated.h"

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

	// Widget component that is responsible for displaying UI.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DrJones")
	TObjectPtr<UWidgetComponent> LetterUI;

	// Display mesh of an letter.
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> LetterMeshComponent;
};
