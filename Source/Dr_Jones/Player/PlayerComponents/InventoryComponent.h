// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Artifacts/Artifact.h"
#include "UI/DrJonesWidgetBase.h"

#include "InventoryComponent.generated.h"

class ADrJonesCharacter;
class ATool;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolAddedDelegate, ATool*, Tool);

UCLASS(Blueprintable, ClassGroup = "Player Components",
	HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class DR_JONES_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	void SetupPlayerInput(UInputComponent* InputComponent);
	
	void ChangeActiveItem(float Value);
	void AddTool(ATool& ToolToAdd);
	void RemoveTool(ATool& ToolToRemove);
	void SetActiveItem(AItem& NewActiveItem);

	void AttachItemToHand(AItem& ItemToAttach);
	void DetachActiveItemFromHand();
	
	void OpenInventory(bool bOpen = true) const;

	UFUNCTION(BlueprintPure)
	AItem* GetItemInHand() const { return ItemInHand; }

	UFUNCTION(BlueprintPure)
	TArray<ATool*> GetTools() const;

	void AddArtifact(AArtifact& Artifact);

	UPROPERTY(BlueprintAssignable, Category = "Tools")
	FOnToolAddedDelegate OnToolAdded;

protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UDrJonesWidgetBase> ItemInfo;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UDrJonesWidgetBase> InventoryMenu;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TArray<TSubclassOf<ATool>> DefaultTools;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<AArtifact> ArtifactInHand;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<AItem> ItemInHand;
	
private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<ATool>> Tools;
	
	UPROPERTY(Transient)
	TObjectPtr<ATool> ActiveTool;
	
	TWeakObjectPtr<ADrJonesCharacter> Owner;
};
