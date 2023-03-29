// Property of Night Shift Games, all rights reserved.

#include "InventoryComponent.h"

/*static*/ uint32 UInventoryComponent::WrapIndexToArray(int32 Index, uint16 ArraySize)
{
	if (ArraySize == 0)
	{
		return 0;
	}
	if (Index < 0)
	{
		return ArraySize - 1;
	}
	if (Index >= ArraySize)
	{
		return 0;
	}
	return Index;
}
