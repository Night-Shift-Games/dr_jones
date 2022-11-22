// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Storage.h"

#include "ToolComponent.generated.h"

class ARuntimeCharacter;
class UTool;

UCLASS()
class DR_JONES_API UToolComponent : public UStorage
{
	GENERATED_BODY()
public:
	UToolComponent();
	
	TArray<UTool*> Tools;
	
	virtual void BeginPlay() override;

	virtual void AddItem(UItem* NewTool) override;
	
	void SwitchItemInHand(UTool* NewTool);
	
	UPROPERTY(EditAnywhere)
	UTool* ActiveItem;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Hand;
	
	void ScrollItem(int Direction);

private:
	ARuntimeCharacter* Player;


};
