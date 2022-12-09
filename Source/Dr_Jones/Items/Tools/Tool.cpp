// Property of Night Shift Games, all rights reserved.

#include "Tool.h"
#include "../../RuntimeCharacter.h"

void UTool::BindTool(ARuntimeCharacter* Player)
{
	Player->OnActionKeyPressed.AddUniqueDynamic(this, &UTool::UseTool);
}

void UTool::UnbindTool(ARuntimeCharacter* Player)
{
	Player->OnActionKeyPressed.RemoveDynamic(this, &UTool::UseTool);
}
