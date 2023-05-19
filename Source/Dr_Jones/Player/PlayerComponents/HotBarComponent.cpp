// Property of Night Shift Games, all rights reserved.

#include "HotBarComponent.h"

#include "ReactionComponent.h"
#include "Utilities.h"
#include "Player/WidgetManager.h"

void UHotBarComponent::AddTool(ATool& ToolToAdd)
{
	Tools.Emplace(&ToolToAdd);
	if (Tools.Num() < 2)
	{
		SetActiveItem(ToolToAdd);
	}
	TOptional<float> None;
	GetOwner<ADrJonesCharacter>()->WidgetManager->RequestWidgetUpdate(HotBarUI, None);
}

void UHotBarComponent::RemoveTool(ATool& ToolToRemove)
{
	Tools.Remove(&ToolToRemove);
}

void UHotBarComponent::SetActiveItem(ATool& NewActiveTool)
{
	if (ActiveTool)
	{
		// TODO: Recreating & Destroying mesh
		ActiveTool->GetMeshComponent()->SetVisibility(false);
	}
	
	ActiveTool = &NewActiveTool;
	ActiveTool->GetMeshComponent()->SetVisibility(true);
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
	
	GetOwner<ADrJonesCharacter>()->WidgetManager->RequestWidgetUpdate(HotBarUI, Value);
	
	int32 ActiveItemID;
	if (Tools.Find(ActiveTool, ActiveItemID))
	{
		SetActiveItem(*Tools[Utilities::WrapIndexToArray(ActiveItemID + Value, Tools)]);
	}
}
