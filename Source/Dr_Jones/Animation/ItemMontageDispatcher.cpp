// Property of Night Shift Games, all rights reserved.


#include "Animation/ItemMontageDispatcher.h"

void UItemMontageDispatcher::Dispatch(AItem& Item, const FName& Action)
{
	OnDispatch(&Item, Action);
}
