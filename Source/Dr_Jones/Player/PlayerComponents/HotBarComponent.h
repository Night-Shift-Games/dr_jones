// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/DrJonesWidgetBase.h"

#include "HotBarComponent.generated.h"

class ADrJonesCharacter;
class ATool;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolAddedDelegate, ATool*, Tool);

UCLASS(Blueprintable, ClassGroup = "Player Components",
	HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
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

	UFUNCTION(BlueprintPure)
	ATool* GetActiveTool() const;

	UFUNCTION(BlueprintPure)
	TArray<ATool*> GetTools() const;

	UPROPERTY(BlueprintAssignable, Category = "Tools")
	FOnToolAddedDelegate OnToolAdded;

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
