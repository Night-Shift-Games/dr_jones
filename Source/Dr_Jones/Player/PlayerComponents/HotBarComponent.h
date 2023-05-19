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
	virtual void BeginPlay() override;
	void SetupPlayerInput(UInputComponent* InputComponent);
	
	void ChangeActiveItem(float Value);
	void AddTool(ATool& ToolToAdd);
	void RemoveTool(ATool& ToolToRemove);
	void SetActiveItem(ATool& NewActiveTool);
	ATool* GetActiveTool() const;

	UFUNCTION(BlueprintPure)
	TArray<ATool*> GetTools() const;
	
protected:
	UPROPERTY(EditAnywhere, Category = "UI");
	TSubclassOf<UDrJonesWidgetBase> HotBarUI;
	
private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<ATool>> Tools;
	
	UPROPERTY(Transient)
	TObjectPtr<ATool> ActiveTool;
	
	TWeakObjectPtr<ADrJonesCharacter> Owner;
};
