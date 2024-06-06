#include "DrJonesWidgetBase.h"

bool UDrJonesWidgetBase::Initialize()
{
	if (!Updater && UpdaterClass)
	{
		Updater = NewObject<UWidgetDataObject>(this, UpdaterClass);
	}
	
	return Super::Initialize();
}

void UDrJonesWidgetBase::SetVisibility(ESlateVisibility InVisibility)
{
	RequestVisibilityChange(InVisibility);
}

void UDrJonesWidgetBase::RequestVisibilityChange_Implementation(ESlateVisibility NewVisibility)
{
	RequestParentVisibilitySet(NewVisibility);
}

void UDrJonesWidgetBase::RequestParentVisibilitySet(ESlateVisibility NewVisibility)
{
	UUserWidget::SetVisibility(NewVisibility);
}

UDrJonesWidgetBase* UDrJonesWidgetBase::GetChildWidget(TSubclassOf<UDrJonesWidgetBase> WidgetClass)
{
	if (!WidgetClass)
	{
		return nullptr;
	}
	UDrJonesWidgetBase* Widget = ChildWidgets.FindRef(WidgetClass);
	if (Widget)
	{
		return Widget;
	}

	const UWorld* World = GetWorld();
	for (TObjectIterator<UDrJonesWidgetBase> Itr; Itr; ++Itr )
	{
		UDrJonesWidgetBase* LiveWidget = *Itr;
		
		if (LiveWidget->GetWorld() != World || !LiveWidget->GetClass()->IsChildOf(WidgetClass))
		{
			continue;
		}
		Widget = LiveWidget; 
		ChildWidgets.Emplace(WidgetClass, Widget);
		break;
	}
	return Widget;
}
