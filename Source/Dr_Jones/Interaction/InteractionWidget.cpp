// Property of Night Shift Games, all rights reserved.

#include "InteractionWidget.h"

#include "Utilities/NightShiftMacros.h"

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
	NS_EARLY(!Cast<UInteractionWidgetDataObject>(Updater)->bShouldBeUpdated);
	
	bHasAltInteraction = Cast<UInteractionWidgetDataObject>(Updater)->bHasAltInteraction;
	bShouldBeVisible = Cast<UInteractionWidgetDataObject>(Updater)->bShouldBeVisible;
	
	Super::UpdateData();

	Updater->ResetValuesAfterUsage();
}
