// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Storage.generated.h"

class UItem;

USTRUCT(BlueprintType) struct FItemStorage
{
	GENERATED_BODY()

	TArray<UItem*> Items;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DR_JONES_API UStorage : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStorage();
	FItemStorage Storage;
protected:
	virtual void BeginPlay() override;
	
	virtual void AddItem(UItem* ItemToAdd);
	virtual void RemoveItem(UItem* ItemToRemove);
		
};
