// Property of Night Shift Games, all rights reserved.

#include "WidgetManager.h"

#include "Blueprint/UserWidget.h"
#include "DrJonesCharacter.h"
#include "UI/DrJonesWidgetBase.h"

void UWidgetManager::BeginPlay()
{
	Super::BeginPlay();
	OwningPlayer = GetOwner<ADrJonesCharacter>();
	OwningController = OwningPlayer->GetController<APlayerController>();
	for (const auto& KV : BeginPlayWidgets)
	{
		AddWidget(KV);
		ShowWidget(KV);
	}
}

void UWidgetManager::AddWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
{
	UDrJonesWidgetBase* NewWidget = CreateWidget<UDrJonesWidgetBase>(OwningController.Get(), WidgetClass);
	Widgets.Emplace(WidgetClass, NewWidget);
}

void UWidgetManager::ShowWidget(const TSubclassOf<UDrJonesWidgetBase> Widget)
{
	if (UDrJonesWidgetBase* WidgetToShow = Widgets.Find(Widget)->Get(); !WidgetToShow->IsInViewport())
	{
		WidgetToShow->AddToViewport();
	}
}

void UWidgetManager::HideWidget(const TSubclassOf<UDrJonesWidgetBase> Widget)
{
	if (UDrJonesWidgetBase* WidgetToHide = Widgets.Find(Widget)->Get(); WidgetToHide->IsInViewport())
	{
		WidgetToHide->RemoveFromViewport();
	}
}

void UWidgetManager::RemoveWidget(const TSubclassOf<UDrJonesWidgetBase> Widget)
{
	if (UUserWidget* WidgetToShow = Widgets.Find(Widget)->Get(); !WidgetToShow->IsInViewport())
	{
		WidgetToShow->AddToViewport();
	}
}

UDrJonesWidgetBase* UWidgetManager::GetWidget(const TSubclassOf<UDrJonesWidgetBase> Widget) const
{
	return Widgets.FindRef(Widget);
}
