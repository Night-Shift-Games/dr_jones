// Property of Night Shift Games, all rights reserved.


#include "ToolComponent.h"
#include "../Items/Tools/Tool.h"
#include "../RuntimeCharacter.h"
UToolComponent::UToolComponent()
{
	Hand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlotComponent"));
}
void UToolComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<ARuntimeCharacter>(GetOwner());
	Hand->AttachToComponent(Player->GetMesh(),FAttachmentTransformRules::KeepRelativeTransform, TEXT("RightHandSocket"));
}

void UToolComponent::AddItem(UItem* NewItem)
{
	UTool* NewTool = Cast<UTool>(NewItem);
	if (!NewTool)
	{
		return;
	}
	NewTool->RegisterComponent();
	Tools.Add(NewTool);
	SwitchItemInHand(NewTool);

}

void UToolComponent::SwitchItemInHand(UTool* NewTool)
{
	// If previous item exist unbind it first before set new one
	if (ActiveItem)
	{
		ActiveItem->UnbindTool(Player);
	}
	ActiveItem = NewTool;
	ActiveItem->BindTool(Player);
	Hand->SetStaticMesh(ActiveItem->ItemMesh);
}

UTool* UToolComponent::GetNextItem()
{
	if (Tools.IsEmpty())
	{
		return nullptr;
	}
	int NextItemID = GetActiveItemID() + 1;
	if (NextItemID >= Tools.Num())
	{
		NextItemID = 0;
	}
	return Tools[NextItemID];
}

UTool* UToolComponent::GetPreviousItem()
{
	if (Tools.IsEmpty())
	{
		return nullptr;
	}
	int NextItemID = GetActiveItemID() - 1;
	if (NextItemID < 0)
	{
		NextItemID = Tools.Num() - 1;
	}
	return Tools[NextItemID];
}

UTool* UToolComponent::GetActiveItem()
{
	return ActiveItem;
}

int32 UToolComponent::GetActiveItemID()
{
	return Tools.Find(ActiveItem);
}

TArray<UTool*> UToolComponent::GetItems()
{
	return Tools;
}

void UToolComponent::ScrollItem(int Direction)
{
	// If array is empty there is no point to continue.
	if (Tools.IsEmpty())
	{
		return;
	}
	
	// Get next or previous tool
	int NextItemID = GetActiveItemID() + Direction;
	
	// Looping array
	if (NextItemID >= Tools.Num())
	{
		NextItemID = 0;
	}
	else if (NextItemID < 0)
	{
		NextItemID = Tools.Num() - 1;
	}

	SwitchItemInHand(Tools[NextItemID]);
}
