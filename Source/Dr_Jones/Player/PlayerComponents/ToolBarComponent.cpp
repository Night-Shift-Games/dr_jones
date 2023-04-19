// Property of Night Shift Games, all rights reserved.

#include "ToolBarComponent.h"

#include "Utilities.h"


void UToolBarComponent::AddTool(ATool& ToolToAdd)
{
	Tools.AddUnique(&ToolToAdd);
	if (Tools.Num() < 2)
	{
		SetActiveItem(ToolToAdd);
	}
}

void UToolBarComponent::RemoveTool(ATool& ToolToRemove)
{
	Tools.Remove(&ToolToRemove);
}

void UToolBarComponent::SetActiveItem(ATool& NewActiveTool)
{
	ActiveTool = &NewActiveTool;
}

ATool* UToolBarComponent::GetActiveTool() const
{
	return ActiveTool;
}

void UToolBarComponent::ChangeActiveItem(const int8 Value)
{
	if (Tools.Num() < 2)
	{
		return;
	}
	int32 ActiveItemID;
	if (Tools.Find(ActiveTool, ActiveItemID))
	{
		SetActiveItem(*Tools[Utilities::WrapIndexToArray(ActiveItemID + Value, Tools)]);
	}
}
