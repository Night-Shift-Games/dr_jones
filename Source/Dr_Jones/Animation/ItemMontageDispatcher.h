// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemMontageDispatcher.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class DR_JONES_API UItemMontageDispatcher : public UObject
{
	GENERATED_BODY()

public:
	void Dispatch(const FName& Action);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDispatch(const FName& Action);
};
