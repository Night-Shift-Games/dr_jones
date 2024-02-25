// Property of Night Shift Games, all rights reserved.

#include "WidgetManager.h"

#include "Blueprint/UserWidget.h"
#include "Framework/Application/NavigationConfig.h"
#include "Player/DrJonesCharacter.h"
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
	// Disables switching focuses by using "Tab" key.
	FNavigationConfig& NavigationConfig = *FSlateApplication::Get().GetNavigationConfig();
	NavigationConfig.bTabNavigation = false;
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
	UDrJonesWidgetBase* WidgetToShow = GetWidget(WidgetClass);
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
	UDrJonesWidgetBase* WidgetToHide = GetWidget(WidgetClass);
	if (!WidgetToHide || !WidgetToHide->IsInViewport())
	{
		return;
	}
	WidgetToHide->RemoveFromParent();
}

void UWidgetManager::SetWidgetVisibility(TSubclassOf<UDrJonesWidgetBase> WidgetClass, ESlateVisibility Visibility) const
{
	UDrJonesWidgetBase* Widget = GetWidget(WidgetClass);
	if (!Widget)
	{
		return;
	}
	Widget->SetVisibility(Visibility);
}

void UWidgetManager::RemoveWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
{
	// TODO: It's not actually removing anything xD
	if (!WidgetClass)
	{
		return;
	}
	if (UUserWidget* WidgetToShow = GetWidget(WidgetClass); !WidgetToShow->IsInViewport())
	{
		WidgetToShow->AddToViewport();
	}
}

UDrJonesWidgetBase* UWidgetManager::GetWidget(const TSubclassOf<UDrJonesWidgetBase> WidgetClass) const
{
	if (!WidgetClass)
	{
		return nullptr;
	}
	UDrJonesWidgetBase* FoundWidget = Widgets.FindRef(WidgetClass);
	if (FoundWidget)
	{
		return FoundWidget;
	}
	// Check active page first if still not found.
	for (auto& KV : Widgets)
	{
		if(KV.Value)
		{
			FoundWidget = KV.Value->GetChildWidget(WidgetClass);
		}
		if (FoundWidget)
		{
			break;
		}
	}
	return FoundWidget;
}

void UWidgetManager::RequestWidgetUpdate(const TSubclassOf<UDrJonesWidgetBase> Widget,
	TOptional<float> AxisValue) const
{
	UDrJonesWidgetBase* WidgetToUpdate = GetWidget(Widget);
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
