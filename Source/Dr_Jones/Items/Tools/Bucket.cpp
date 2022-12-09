// Property of Night Shift Games, all rights reserved.


#include "Bucket.h"

void UBucket::UseTool()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 15, FColor::Red, TEXT("Bucket in use"));
}
