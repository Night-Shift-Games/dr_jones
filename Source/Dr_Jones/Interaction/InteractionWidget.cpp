// Property of Night Shift Games, all rights reserved.

#include "InteractionWidget.h"

bool UInteractionWidget::Initialize()
{
	if (!UpdaterClass)
	{
		UpdaterClass = UInteractionWidgetDataObject::StaticClass();
	}
	return Super::Initialize();
}

void UInteractionWidget::UpdateData()
{
	HasAltInteraction = Cast<UInteractionWidgetDataObject>(Updater)->HasAltInteraction;
	
	Super::UpdateData();

	Updater->ResetValuesAfterUsage();
}
