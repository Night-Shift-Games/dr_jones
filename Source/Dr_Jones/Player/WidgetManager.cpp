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
	if (!WidgetClass)
	{
		return;
	}
	UDrJonesWidgetBase* NewWidget = CreateWidget<UDrJonesWidgetBase>(OwningController.Get(), WidgetClass);
	Widgets.Emplace(WidgetClass, NewWidget);
}

void UWidgetManager::ShowWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
{
	if (!WidgetClass)
	{
		return;
	}
	UDrJonesWidgetBase* WidgetToShow = Widgets.FindRef(WidgetClass);
	if (!WidgetToShow || WidgetToShow->IsInViewport())
	{
		return;
	}
	WidgetToShow->AddToViewport();
}

void UWidgetManager::HideWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
{
	if (!WidgetClass)
	{
		return;
	}
	UDrJonesWidgetBase* WidgetToHide = Widgets.FindRef(WidgetClass);
	if (!WidgetToHide || !WidgetToHide->IsInViewport())
	{
		return;
	}
	WidgetToHide->RemoveFromParent();
}

void UWidgetManager::RemoveWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
{
	// TODO: It's not actually removing anything xD
	if (!WidgetClass)
	{
		return;
	}
	if (UUserWidget* WidgetToShow = Widgets.FindRef(WidgetClass); !WidgetToShow->IsInViewport())
	{
		WidgetToShow->AddToViewport();
	}
}

UDrJonesWidgetBase* UWidgetManager::GetWidget(const TSubclassOf<UDrJonesWidgetBase> Widget) const
{
	return Widgets.FindRef(Widget);
}

void UWidgetManager::RequestWidgetUpdate(const TSubclassOf<UDrJonesWidgetBase> Widget,
	TOptional<float> AxisValue) const
{
	UDrJonesWidgetBase* WidgetToUpdate = Widgets.FindRef(Widget);
	if (!WidgetToUpdate)
	{
		return;
	}
	if (AxisValue.IsSet())
	{
		WidgetToUpdate->UpdateDataAxis(AxisValue.GetValue());	
	}
	else
	{
		WidgetToUpdate->UpdateData();
	}
}
