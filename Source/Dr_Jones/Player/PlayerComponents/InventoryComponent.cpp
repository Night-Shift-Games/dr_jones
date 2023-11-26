// Property of Night Shift Games, all rights reserved.

#include "InventoryComponent.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Items/Tools/Tool.h"
#include "Player/WidgetManager.h"
#include "ReactionComponent.h"
#include "Utilities.h"

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner<ADrJonesCharacter>();
	UWorld* World = GetWorld();
	check (Owner.Get())
	check (World)
	for (TSubclassOf<ATool> ToolClass : DefaultTools)
	{
		ATool* NewTool = World->SpawnActor<ATool>(ToolClass);
		// TODO: XDDDDD
		NewTool->PickUp(Owner.Get());
	}
}

void UInventoryComponent::SetupPlayerInput(UInputComponent* InputComponent)
{
	InputComponent->BindAxis(TEXT("Scroll"), this, &UInventoryComponent::ChangeActiveItem);
}

void UInventoryComponent::ChangeActiveItem(float Value)
{
	if (Value == 0 || Tools.Num() < 2)
	{
		return;
	}
	
	int32 ActiveItemID;
	if (Tools.Find(ActiveTool, ActiveItemID))
	{
		SetActiveItem(*Tools[Utilities::WrapIndexToArray(ActiveItemID + Value, Tools)]);
	}
}

void UInventoryComponent::AddTool(ATool& ToolToAdd)
{
	Tools.Emplace(&ToolToAdd);
	if (Tools.Num() < 2)
	{
		SetActiveItem(ToolToAdd);
	}
	const TOptional<float> None;
	Owner->WidgetManager->RequestWidgetUpdate(HotBarUI, None);

	OnToolAdded.Broadcast(&ToolToAdd);
}

void UInventoryComponent::RemoveTool(ATool& ToolToRemove)
{
	Tools.Remove(&ToolToRemove);
}

void UInventoryComponent::SetActiveItem(ATool& NewActiveTool)
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
	Owner->GetWidgetManager()->RequestWidgetUpdate(HotBarUI, NullOpt);
}

ATool* UInventoryComponent::GetActiveTool() const
{
	return ActiveTool;
}

TArray<ATool*> UInventoryComponent::GetTools() const
{
	return Tools;
}
