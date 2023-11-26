// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Item.generated.h"

class UActionComponent;
class UInteractableComponent;

UCLASS(HideCategories = (Rendering, Replication, Collision, HLOD, Input, "Code View", Cooking, Actor))
class DR_JONES_API AItem : public AActor
{
	GENERATED_BODY()
	
public:
	AItem();
	virtual UMeshComponent* GetMeshComponent() const;

	TSubclassOf<UAnimInstance> GetItemAnimation() const;

	UTexture2D* GetItemIcon() const;

	void EnableInteraction(bool bEnabled);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DrJones|WorldComponents")
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DrJones|WorldComponents")
	TObjectPtr<UActionComponent> ActionComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Item")
	FName AttachmentSocket = TEXT("RightHandSocket");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrJones|Animation")
	TSubclassOf<UAnimInstance> ItemAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrJones|UI")
	TObjectPtr<UTexture2D> ItemIcon;
};

FORCEINLINE TSubclassOf<UAnimInstance> AItem::GetItemAnimation() const
{
	return ItemAnimation;
}
