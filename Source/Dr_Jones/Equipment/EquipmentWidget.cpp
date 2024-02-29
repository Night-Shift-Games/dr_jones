// Property of Night Shift Games, all rights reserved.

#include "EquipmentWidget.h"

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
		Algo::Copy(*CastedUpdater->Letters.GetValue(), Letters);
		Algo::Copy(*CastedUpdater->Tools.GetValue(), Tools);
	}
	
	Super::UpdateData();
}