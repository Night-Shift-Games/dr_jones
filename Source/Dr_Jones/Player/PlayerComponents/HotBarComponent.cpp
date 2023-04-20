// Property of Night Shift Games, all rights reserved.

#include "HotBarComponent.h"

#include "ReactionComponent.h"
#include "Utilities.h"

void UHotBarComponent::AddTool(ATool* ToolToAdd)
{
	Tools.Emplace(ToolToAdd);
	if (Tools.Num() < 2)
	{
		SetActiveItem(*ToolToAdd);
	}
}

void UHotBarComponent::RemoveTool(ATool& ToolToRemove)
{
	Tools.Remove(&ToolToRemove);
}

void UHotBarComponent::SetActiveItem(ATool& NewActiveTool)
{
	ActiveTool = &NewActiveTool;
	GetOwner<ADrJonesCharacter>()->ReactionComponent->SetActiveItem(NewActiveTool);
}

ATool* UHotBarComponent::GetActiveTool() const
{
	return ActiveTool;
}

void UHotBarComponent::ChangeActiveItem(const int8 Value)
{
	if (Tools.Num() < 2)
	{
		return;
	}
//	int32 ActiveItemID;
//	if (Tools.Find(ActiveTool, ActiveItemID))
//	{
//		SetActiveItem(*Tools[Utilities::WrapIndexToArray(ActiveItemID + Value, Tools)]);
//	}
}
