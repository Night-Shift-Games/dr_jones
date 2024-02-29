// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/DrJonesWidgetBase.h"

#include "EquipmentWidget.generated.h"

class UEquipmentComponent;
class AItem;
class ALetter;
class ATool;

UCLASS()
class DR_JONES_API UEquipmentWidget : public UDrJonesWidgetBase
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	virtual void UpdateData() override;
	
public:
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<ATool>> Tools;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<ALetter>> Letters;

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<AItem>> QuickSlotsItems;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UEquipmentComponent> OwningEquipment;
};

UCLASS()
class UEquipmentWidgetDataObject : public UWidgetDataObject
{
	GENERATED_BODY()
	
public:
	TOptional<TArray<TObjectPtr<ATool>>*> Tools;
	TOptional<TArray<TObjectPtr<ALetter>>*> Letters;
	TOptional<TArray<TObjectPtr<AItem>>*> QuickSlotsItems;
};
