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
	UTool* GetNextItem();

	UFUNCTION(BlueprintPure)
	UTool* GetPreviousItem();

	UFUNCTION(BlueprintPure)
	UTool* GetActiveItem();
	
	UFUNCTION(BlueprintPure)
	int32 GetActiveItemID();

	UFUNCTION(BlueprintPure)
	TArray<UTool*> GetItems();
	
	virtual void AddItem(UItem* NewTool) override;

	void ScrollItem(int Direction);

	UPROPERTY(EditAnywhere)
	UTool* ActiveItem;

	UPROPERTY(EditAnywhere)
	TArray<UTool*> Tools;


	UStaticMeshComponent* GetHand() { return Hand; };

private:
	UStaticMeshComponent* Hand;
	ARuntimeCharacter* Player;


};
