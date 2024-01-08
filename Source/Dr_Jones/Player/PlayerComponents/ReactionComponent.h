// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "ReactionComponent.generated.h"

class AItem;
class UActionComponent;

UCLASS(ClassGroup = "Player Components", meta = (BlueprintSpawnableComponent),
	HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class DR_JONES_API UReactionComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	void CallAction();
	void CallSecondaryAction();
	void SetActiveItem(AItem* NewActiveItem);
	
	UFUNCTION(BlueprintPure, Category = "DrJones")
	AItem* GetActiveItem() const;

	void SetupPlayerInput(UInputComponent* InputComponent);
	
protected:
	TTuple<TObjectPtr<AItem>, TObjectPtr<UActionComponent>> ActiveItem;
};
