// Property of Night Shift Games, all rights reserved.

#include "WidgetManager.h"

void UWidgetManager::AddWidget(const TSubclassOf<UUserWidget> WidgetClass)
{
	UUserWidget* NewWidget = CreateWidget(GetWorld(), WidgetClass);
	Widgets.Add(WidgetClass, NewWidget);
}

void UWidgetManager::ShowWidget(const TSubclassOf<UUserWidget> Widget)
{
	if (UUserWidget* WidgetToShow = Widgets.Find(Widget)->Get(); !WidgetToShow->IsInViewport())
	{
		WidgetToShow->AddToViewport();
	}
}

void UWidgetManager::HideWidget(const TSubclassOf<UUserWidget> Widget)
{
	if (UUserWidget* WidgetToHide = Widgets.Find(Widget)->Get(); WidgetToHide->IsInViewport())
	{
		WidgetToHide->RemoveFromViewport();
	}
}
