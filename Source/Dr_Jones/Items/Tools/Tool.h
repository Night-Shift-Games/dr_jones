// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Player/DrJonesCharacter.h"

#include "Tool.generated.h"

class UActionComponent;
class UInteractableComponent;

UCLASS()
class DR_JONES_API ATool : public AItem
{
	GENERATED_BODY()
public:
	ATool();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable, Category = "DrJones")
	void PickUp(ADrJonesCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayToolMontage(FName MontageName);
	
protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UActionComponent> ActionComponent;

	UPROPERTY(Transient)
	TObjectPtr<ADrJonesCharacter> OwningPlayer;
};
