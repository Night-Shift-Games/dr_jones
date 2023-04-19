// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ToolBarComponent.generated.h"

class ADrJonesCharacter;
class ATool;

UCLASS(Blueprintable)
class DR_JONES_API UToolBarComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	TArray<TObjectPtr<ATool>> Tools;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tools")
	TObjectPtr<ATool> ActiveTool;
	
	void AddTool(ATool& ToolToAdd);
	void RemoveTool(ATool& ToolToRemove);
	void SetActiveItem(ATool& NewActiveTool);
	ATool* GetActiveTool() const;
	void ChangeActiveItem(const int8 Value);
};
