// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "../Item.h"
#include "../../Interactive/InteractiveObject.h"

#include "Artefact.generated.h"

UCLASS(Blueprintable)
class DR_JONES_API UArtefact : public UItem, public IInteractiveObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	FString Description;

	UPROPERTY(EditAnywhere)
	FString Rarity;

	virtual void BeginPlay() override;
	
	virtual void Interact(APawn* Indicator) override;

	void Take(APawn* Taker);
};

