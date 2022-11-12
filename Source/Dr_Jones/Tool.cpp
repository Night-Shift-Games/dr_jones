// Property of Night Shift Games, all rights reserved.

#include "Tool.h"
#include "RuntimeCharacter.h"

void UTool::UseItem()
{
}

void UTool::SetupTool()
{
	if (Mesh) StaticMeshComponent->SetStaticMesh(Mesh);
}
