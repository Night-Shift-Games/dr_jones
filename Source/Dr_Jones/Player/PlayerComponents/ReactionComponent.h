// Property of Night Shift Games, all rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "ReactionComponent.generated.h"

class UActionComponent;

UCLASS(ClassGroup = "Player Components", meta = (BlueprintSpawnableComponent))
class DR_JONES_API UReactionComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	void CallAction();
	void SetActiveItem(AItem& NewActiveItem);
	
	UFUNCTION(BlueprintPure, Category = "DrJones")
	AItem* GetActiveItem() const;

	void SetupPlayerInput(UInputComponent* InputComponent);
	
protected:
	TTuple<TObjectPtr<AItem>, TObjectPtr<UActionComponent>> ActiveItem;
};
