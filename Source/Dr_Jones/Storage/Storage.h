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


UCLASS( ClassGroup="Storage", meta = (BlueprintSpawnableComponent))
class DR_JONES_API UStorage : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStorage();

	UFUNCTION(BlueprintPure)
	FItemStorage GetStorage();
	
	UFUNCTION(BlueprintCallable)
	virtual void AddItem(UItem* ItemToAdd);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveItem(UItem* ItemToRemove);
protected:
	FItemStorage Storage;
		

};
