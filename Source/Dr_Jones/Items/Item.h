// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.generated.h"


UCLASS( ClassGroup=(Item), meta=(BlueprintSpawnableComponent) )
class DR_JONES_API UItem : public USceneComponent
{
	GENERATED_BODY()

public:	
	UItem();
	
	UPROPERTY(EditDefaultsOnly)
	FName ItemName;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> ItemImage;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* ItemMesh;

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetItemImage();

protected:
	UStaticMeshComponent* StaticMeshComponent;

};
