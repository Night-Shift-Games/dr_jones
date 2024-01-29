#include "DrJonesWidgetBase.h"

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
	for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr )
	{
		UUserWidget* LiveWidget = *Itr;
		
		if (LiveWidget->GetWorld() != World || !LiveWidget->GetClass()->IsChildOf(WidgetClass))
		{
			continue;
		}
		Widget = Cast<UDrJonesWidgetBase>(LiveWidget);
		ChildWidgets.Emplace(WidgetClass, Widget);
		break;
	}
	return Widget;
}
