// Property of Night Shift Games, all rights reserved.

#include "HotBarComponent.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Items/Tools/Tool.h"
#include "Player/WidgetManager.h"
#include "ReactionComponent.h"
#include "Utilities.h"

void UHotBarComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner<ADrJonesCharacter>();
}

void UHotBarComponent::SetupPlayerInput(UInputComponent* InputComponent)
{
	InputComponent->BindAxis("Scroll", this, &UHotBarComponent::ChangeActiveItem);
}

void UHotBarComponent::ChangeActiveItem(float Value)
{
	if (Value == 0 || Tools.Num() < 2)
	{
		return;
	}
	
	Owner->GetWidgetManager()->RequestWidgetUpdate(HotBarUI, Value);
	
	int32 ActiveItemID;
	if (Tools.Find(ActiveTool, ActiveItemID))
	{
		SetActiveItem(*Tools[Utilities::WrapIndexToArray(ActiveItemID + Value, Tools)]);
	}
}

void UHotBarComponent::AddTool(ATool& ToolToAdd)
{
	Tools.Emplace(&ToolToAdd);
	if (Tools.Num() < 2)
	{
		SetActiveItem(ToolToAdd);
	}
	const TOptional<float> None;
	Owner->WidgetManager->RequestWidgetUpdate(HotBarUI, None);
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
	Owner->ReactionComponent->SetActiveItem(NewActiveTool);

	Owner->CharacterAnimationComponent->SetActiveItemAnimation(ActiveTool->GetItemAnimation());
}

ATool* UHotBarComponent::GetActiveTool() const
{
	return ActiveTool;
}

TArray<ATool*> UHotBarComponent::GetTools() const
{
	return Tools;
}
