#include "DrJonesWidgetBase.h"

bool UDrJonesWidgetBase::Initialize()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return false;
	}
	
	LowLevelRename(GetClass()->GetFName());
	
	return Super::Initialize();
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
	if (UWidget* FoundWidget = GetWidgetFromName(WidgetClass->GetFName()))
	{
		Widget = Cast<UDrJonesWidgetBase>(FoundWidget);
		ChildWidgets.Emplace(WidgetClass, Widget);
	}
	return Widget;
}
