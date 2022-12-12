// Property of Night Shift Games, all rights reserved.


#include "ToolPickupActor.h"
#include "../Items/Tools/Shovel.h"

void AToolPickupActor::OnConstruction(const FTransform& Transform)
{
	if (ToolMesh)
	{
		StaticMeshComponent->SetStaticMesh(ToolMesh);
	}
}

void AToolPickupActor::Interact(APawn* Indicator)
{
	UTool* NewTool = NewObject<UTool>(Indicator, ToolClass);
	NewTool->ItemMesh = ToolMesh;
	NewTool->ItemImage = ToolIcon;
	NewTool->ItemName = ToolName;
	UShovel* Shovel = Cast<UShovel>(NewTool);
	if (Shovel)
	{
		Shovel->ShovelDirt = ShovelDirt;
	}
	if (ARuntimeCharacter* Player = Cast<ARuntimeCharacter>(Indicator))
	{
		if (Player->ToolComponent)
		{
			Player->ToolComponent->AddItem(NewTool);
		}
	}
	this->Destroy();
}

FString AToolPickupActor::GetInteractSentence()
{
	return InteractionSentence + ToolName.ToString();
}
