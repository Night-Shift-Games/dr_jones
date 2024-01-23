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
	
	virtual UMeshComponent* GetMeshComponent() const { return FindComponentByClass<UMeshComponent>(); }
	TSubclassOf<UAnimInstance> GetItemAnimation() const { return ItemAnimation; }
	UTexture2D* GetItemIcon() const { return ItemIcon; }
	FName GetItemAttachmentSocket() const { return AttachmentSocket; }

	void SetInteractionEnabled(bool bEnabled) const;
	virtual void SetHandAttachmentPhysics();
	virtual void SetWorldPhysics();

	void SetupItemInHandProperties();
	void SetupItemWorldProperties();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DrJones|WorldComponents")
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DrJones|WorldComponents")
	TObjectPtr<UActionComponent> ActionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName AttachmentSocket = TEXT("RightHandSocket");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrJones|Animation")
	TSubclassOf<UAnimInstance> ItemAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DrJones|UI")
	TObjectPtr<UTexture2D> ItemIcon;
};
