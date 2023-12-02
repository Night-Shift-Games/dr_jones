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
	if (UWidget* FoundWidget = GetWidgetFromName(WidgetClass->GetFName()))
	{
		Widget = Cast<UDrJonesWidgetBase>(FoundWidget);
		ChildWidgets.Emplace(WidgetClass, Widget);
	}
	return Widget;
}
