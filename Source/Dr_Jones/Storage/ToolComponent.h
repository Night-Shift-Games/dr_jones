// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Storage.h"

#include "ToolComponent.generated.h"

class ARuntimeCharacter;
class UTool;

UCLASS(ClassGroup = "Storage", meta = (BlueprintSpawnableComponent))
class DR_JONES_API UToolComponent : public UStorage
{
	GENERATED_BODY()
public:
	UToolComponent();
	virtual void BeginPlay() override;
	virtual void AddItem(UClass* ItemToAdd);
	void SwitchItemInHand(UTool* NewTool);
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ItemHoldInHand;
private:
	ARuntimeCharacter* Owner;


};
