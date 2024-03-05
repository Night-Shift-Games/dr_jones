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
	UEquipmentWidgetDataObject* CastedUpdater = Cast<UEquipmentWidgetDataObject>(Updater);
	if (!CastedUpdater)
	{
		Super::UpdateData();
		return;
	}
	if (CastedUpdater->UpdatingEquipment.IsSet())
	{
		OwningEquipment = CastedUpdater->UpdatingEquipment.GetValue().Get();
	}
	if (CastedUpdater->Letters.IsSet())
	{
		Letters.Reset();
		Algo::Copy(*CastedUpdater->Letters.GetValue(), Letters);
	}
	if (CastedUpdater->Tools.IsSet())
	{
		Tools.Reset();
		Algo::Copy(*CastedUpdater->Tools.GetValue(), Tools);
	}
	if (CastedUpdater->QuickSlotsItems.IsSet())
	{
		QuickSlotsItems.Reset();
		Algo::Copy(*CastedUpdater->QuickSlotsItems.GetValue(), QuickSlotsItems);
	}
	Super::UpdateData();
}

void UEquipmentWidget::AddItemToSlot(AItem* Item, int Index)
{
	if (!OwningEquipment)
	{
		return;
	}
	if (!Item)
	{
		return;
	}
	OwningEquipment->AddToQuickSlot(*Item, Index);
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
