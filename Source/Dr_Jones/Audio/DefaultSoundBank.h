// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "DefaultSoundBank.generated.h"

UCLASS()
class DR_JONES_API UDefaultSoundBank : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<USoundBase> EquipSound;

	UPROPERTY(EditAnywhere,	BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<USoundBase> PlaceArtifactSound;
};
