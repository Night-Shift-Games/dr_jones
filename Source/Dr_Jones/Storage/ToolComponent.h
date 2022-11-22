// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Storage.h"

#include "ToolComponent.generated.h"

class ARuntimeCharacter;
class UTool;

UCLASS(Blueprintable)
class DR_JONES_API UToolComponent : public UStorage
{
	GENERATED_BODY()
public:
	UToolComponent();

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void SwitchItemInHand(UTool* NewTool);
	
	UFUNCTION(BlueprintPure)
	UTool* GetActiveItem();
	
	UFUNCTION(BlueprintPure)
	int32 GetActiveItemID();

	UFUNCTION(BlueprintPure)
	TArray<UTool*> GetItems();
	
	UFUNCTION(BlueprintCallable)
	virtual void AddItem(UItem* NewTool) override;

	void ScrollItem(int Direction);

private:
	UStaticMeshComponent* Hand;
	ARuntimeCharacter* Player;
	UTool* ActiveItem;
	TArray<UTool*> Tools;

};
