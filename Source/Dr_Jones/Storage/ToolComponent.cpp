// Property of Night Shift Games, all rights reserved.


#include "ToolComponent.h"
#include "../Items/Tools/Tool.h"
#include "../RuntimeCharacter.h"
UToolComponent::UToolComponent()
{
	ItemHoldInHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlotComponent"));
}
void UToolComponent::BeginPlay()
{
	Owner = Cast<ARuntimeCharacter>(GetOwner());
	ItemHoldInHand->AttachToComponent(Owner->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
}

void UToolComponent::AddItem(UClass* ItemToAdd)
{
	UTool* ToolToAdd = NewObject<UTool>(this, ItemToAdd);
	ToolToAdd->SetupTool();
	ItemHoldInHand->SetStaticMesh(ToolToAdd->Mesh);
}

void UToolComponent::SwitchItemInHand(UTool* NewTool)
{

}
