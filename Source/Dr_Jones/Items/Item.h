// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.generated.h"


UCLASS( ClassGroup=(Item), meta=(BlueprintSpawnableComponent) )
class DR_JONES_API UItem : public UStaticMeshComponent
{
	GENERATED_BODY()

public:	
	UItem();
	
	UPROPERTY(EditDefaultsOnly, Category = "Item", meta = (DisplayPriority = 1))
	FName ItemName;
	
	UPROPERTY(EditAnywhere, Category = "Item", meta = (DisplayPriority = 2))
	TObjectPtr<UTexture2D> ItemImage;

	UPROPERTY(EditAnywhere, Category = "Item", meta = (DisplayPriority = 3, MultiLine = true))
	FString Description;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	UStaticMesh* ItemMesh;

	UFUNCTION(BlueprintCallable)
	FName GetItemDescription();

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetItemImage();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

};
