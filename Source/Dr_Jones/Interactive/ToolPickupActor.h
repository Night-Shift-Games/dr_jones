// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveActor.h"
#include "GameFramework/Actor.h"
#include "../Items/Tools/Tool.h"
#include ".././RuntimeCharacter.h"
#include "ToolPickupActor.generated.h"

UCLASS()
class DR_JONES_API AToolPickupActor : public AInteractiveActor
{
	GENERATED_BODY()

public:

	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTool> ToolClass;

	UPROPERTY(EditAnywhere)
	UStaticMesh* ToolMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ToolIcon;

	UPROPERTY(EditAnywhere)
	FName ToolName;

	virtual void Interact(APawn* Indicator) override;

	virtual FString GetInteractSentence() override;

};
