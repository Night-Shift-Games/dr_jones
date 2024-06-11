// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CampEntity.h"

#include "PostBox.generated.h"

UCLASS(Blueprintable)
class DR_JONES_API APostBox : public ACampEntity
{
	GENERATED_BODY()

public:
	APostBox();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> PostBoxMesh;
};
