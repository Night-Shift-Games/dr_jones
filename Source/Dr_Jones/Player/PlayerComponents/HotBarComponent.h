// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Tools/Tool.h"
#include "UI/DrJonesWidgetBase.h"

#include "HotBarComponent.generated.h"

class ADrJonesCharacter;
class ATool;

UCLASS(Blueprintable)
class DR_JONES_API UHotBarComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	void AddTool(ATool& ToolToAdd);
	void RemoveTool(ATool& ToolToRemove);
	void SetActiveItem(ATool& NewActiveTool);
	ATool* GetActiveTool() const;

	void ChangeActiveItem(int8 Value);
	
public:
	UPROPERTY()
	TArray<TObjectPtr<ATool>> Tools;

	UPROPERTY(EditAnywhere, Category="UI");
	TSubclassOf<UDrJonesWidgetBase> HotBarUI;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tools")
	TObjectPtr<ATool> ActiveTool;
};
