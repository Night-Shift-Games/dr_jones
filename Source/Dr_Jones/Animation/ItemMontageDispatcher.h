// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemMontageDispatcher.generated.h"

class AItem;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class DR_JONES_API UItemMontageDispatcher : public UObject
{
	GENERATED_BODY()

public:
	void Dispatch(AItem& Item, const FName& Action);

	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void OnDispatch(AItem* Item, const FName& Action);

public:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;
};
