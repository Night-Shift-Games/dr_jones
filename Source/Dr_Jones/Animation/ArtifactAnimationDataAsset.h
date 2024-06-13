// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "ArtifactAnimationDataAsset.generated.h"

UCLASS()
class DR_JONES_API UArtifactAnimationDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAnimInstance> HoldLayer;

	UPROPERTY(EditAnywhere)
	FName AttachBone = TEXT("item_r");

	UPROPERTY(EditAnywhere)
	FTransform AttachOffset = FTransform::Identity;
};
