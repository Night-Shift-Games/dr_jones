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
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10, FColor::Red, "TestBeginPlayTC");
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
	if (ActiveItem)
	{
		ActiveItem->UnbindTool(Player);
	}
	ActiveItem = NewTool;
	ActiveItem->BindTool(Player);
	Hand->SetStaticMesh(ActiveItem->ItemMesh);
}

void UToolComponent::ScrollItem(int Direction)
{
	if (Tools.IsEmpty())
	{
		return;
	}
	int NextItemID = (Tools.Find(ActiveItem) + Direction);
	
	if (NextItemID >= Tools.Num()) NextItemID = 0;
	else if (NextItemID < 0) NextItemID = Tools.Num() - 1;
	
	SwitchItemInHand(Tools[NextItemID]);
}
