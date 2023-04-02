// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"

#include "Tool.generated.h"

class UInteractableComponent;

UCLASS()
class DR_JONES_API ATool : public AItem
{
	GENERATED_BODY()
public:
	ATool();
	UFUNCTION(BlueprintCallable)
	virtual void UseToolPrimaryAction() {};

	UFUNCTION(BlueprintCallable)
	virtual void UseToolSecondaryAction() {};

	virtual void BindTool(ADrJonesCharacter& Player);
	virtual void UnbindTool(ADrJonesCharacter& Player);

	virtual void PickUp();

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UInteractableComponent> InteractableComponent;
};
