// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Player/DrJonesCharacter.h"

#include "Tool.generated.h"

class UActionComponent;
class UInteractableComponent;

inline TAutoConsoleVariable CVarToolDebug(
	TEXT("NS.ToolDebug"),
	false,
	TEXT("Draw debug shapes for the tools' functionality"),
	ECVF_Cheat
);

UCLASS()
class DR_JONES_API ATool : public AItem
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable, Category = "DrJones")
	void PickUp(ADrJonesCharacter* Player);
	
	UAnimMontage* FindActionMontage(const FName& MontageName) const;
	
	virtual void OnRemovedFromEquipment() override;
	virtual void OnEquip() override;
	virtual void OnUnequip() override;
	
protected:
	UPROPERTY(Transient)
	TObjectPtr<ADrJonesCharacter> OwningPlayer;
};
