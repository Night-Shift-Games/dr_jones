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
	Cast<ARuntimeCharacter>(Indicator)->ToolComponent->AddItem(NewTool);
	this->Destroy();
}
