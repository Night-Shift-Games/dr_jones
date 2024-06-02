// Property of Night Shift Games, all rights reserved.


#include "InteractionHintWidget.h"

#include "Utilities/NightShiftMacros.h"

bool UInteractionHintWidget::Initialize()
{
	if (!UpdaterClass)
	{
		UpdaterClass = UInteractionHintWidgetDataObject::StaticClass();
	}
	return Super::Initialize();
}

void UInteractionHintWidget::UpdateData()
{
	bShouldUpdateVisuals = Cast<UInteractionHintWidgetDataObject>(Updater)->bShouldUpdateVisuals;
	bShouldBeVisible = Cast<UInteractionHintWidgetDataObject>(Updater)->bShouldBeVisible;
	bHasAltInteraction = Cast<UInteractionHintWidgetDataObject>(Updater)->bHasAltInteraction;
	
	Super::UpdateData();

	Updater->ResetValuesAfterUsage();
}

