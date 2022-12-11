// Property of Night Shift Games, all rights reserved.


#include "ToolPickupActor.h"

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
	if (ARuntimeCharacter* Player = Cast<ARuntimeCharacter>(Indicator))
	{
		if (Player->ToolComponent)
		{
			Player->ToolComponent->AddItem(NewTool);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15, FColor::Red, TEXT("Missing Tool Component"));
		}
	}
	this->Destroy();
}

FString AToolPickupActor::GetInteractSentence()
{
	return InteractionSentence + ToolName.ToString();
}
