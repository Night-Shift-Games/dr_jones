// Property of Night Shift Games, all rights reserved.


#include "Animation/ItemMontageDispatcher.h"

void UItemMontageDispatcher::Dispatch(const FName& Action)
{
	OnDispatch(Action);
}
