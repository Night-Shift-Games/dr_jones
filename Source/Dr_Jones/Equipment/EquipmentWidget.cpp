// Property of Night Shift Games, all rights reserved.

#include "EquipmentWidget.h"

#include "EquipmentComponent.h"

bool UEquipmentWidget::Initialize()
{
	if (!UpdaterClass)
	{
		UpdaterClass = UEquipmentWidgetDataObject::StaticClass();
	}
	
	return Super::Initialize();
}

void UEquipmentWidget::UpdateData()
{
	if (UEquipmentWidgetDataObject* CastedUpdater = Cast<UEquipmentWidgetDataObject>(Updater))
	{
		Letters.Reset();
		Tools.Reset();
		QuickSlotsItems.Reset();
		Algo::Copy(*CastedUpdater->Letters.GetValue(), Letters);
		Algo::Copy(*CastedUpdater->Tools.GetValue(), Tools);
		Algo::Copy(*CastedUpdater->QuickSlotsItems.GetValue(), QuickSlotsItems);
	}
	
	Super::UpdateData();
}

void UEquipmentWidget::AddItemToSlot(AItem* Item)
{
	if (!OwningEquipment)
	{
		return;
	}
	if (!Item)
	{
		return;
	}
	OwningEquipment->AddToQuickSlot(*Item);
}

void UEquipmentWidget::RemoveItemFromQuickSlot(AItem* Item)
{
	if (!OwningEquipment)
	{
		return;
	}
	if (!Item)
	{
		return;
	}
	OwningEquipment->RemoveFromQuickSlot(*Item);
}
