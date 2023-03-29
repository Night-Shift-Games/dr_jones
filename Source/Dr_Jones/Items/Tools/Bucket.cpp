// Property of Night Shift Games, all rights reserved.


#include "Bucket.h"

void ABucket::UseToolPrimaryAction()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15, FColor::Red, TEXT("Bucket in use"));
}
