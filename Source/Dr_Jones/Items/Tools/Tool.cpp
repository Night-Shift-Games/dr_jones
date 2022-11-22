// Property of Night Shift Games, all rights reserved.

#include "Tool.h"
#include "../../RuntimeCharacter.h"

// Virtual function
void UTool::UseItem() {}

void UTool::BindTool(ARuntimeCharacter* Player)
{
	Player->OnActionKeyPressed.AddUniqueDynamic(this, &UTool::UseItem);
}

void UTool::UnbindTool(ARuntimeCharacter* Player)
{
	Player->OnActionKeyPressed.RemoveDynamic(this, &UTool::UseItem);
}
