// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "EquipmentComponent.generated.h"

class AArtifact;
class ADrJonesCharacter;
class AItem;
class ALetter;
class ATool;
class UDrJonesWidgetBase;
class UInputAction;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnToolAddedDelegate, ATool*, Tool);

UCLASS(Blueprintable, ClassGroup = "Player Components",
	HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class DR_JONES_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	void SetupPlayerInput(UEnhancedInputComponent* EnhancedInputComponent);

	UFUNCTION(BlueprintPure, Category = "DrJones|Equipment")
	AItem* GetItemInHand() const { return ItemInHand; }

	UFUNCTION(BlueprintPure, Category = "DrJones|Equipment")
	TArray<ATool*> GetTools() const { return Tools; }

	UFUNCTION(BlueprintPure, Category = "DrJones|Equipment")
	TArray<ALetter*> GetQuestItems() const { return QuestItems; }
	
	UFUNCTION(BlueprintCallable, Category = "DrJones|Equipment")
	void AddItem(AItem* ItemToAdd);

	UFUNCTION(BlueprintCallable, Category = "DrJones|Equipment")
	void EquipItemByClass(TSubclassOf<AItem> ItemClass);
	
	UFUNCTION(BlueprintCallable, Category = "DrJones|Equipment")
	void EquipItem(AItem* NewActiveItem);

	UFUNCTION(BlueprintCallable, Category = "DrJones|Equipment")
	void UnequipItem();

	UFUNCTION(BlueprintCallable, Category = "DrJones|Equipment")
	AItem* TakeOutItemInHand();

	bool CanPickUpItem() const;

	void CallPrimaryItemAction();
	void CallSecondaryItemAction();
	
	void ChangeActiveItem(const FInputActionValue& InputActionValue);
	void AttachItemToHand(AItem& ItemToAttach);
	void DetachItemFromHand(AItem& ItemToDetach);
	
	void OpenEquipmentWheel(const FInputActionValue& InputActionValue);

	UPROPERTY(BlueprintAssignable, Category = "Tools")
	FOnToolAddedDelegate OnToolAdded;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Tools")
	TArray<TSubclassOf<ATool>> DefaultTools;
	
	// UI
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UDrJonesWidgetBase> ItemInfo;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UDrJonesWidgetBase> InventoryMenu;
	
	// Input Actions
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ChangeItemAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> OpenEquipmentAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SecondaryAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> UnequipItemAction;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TObjectPtr<AItem> ItemInHand;
	
	UPROPERTY(SaveGame)
	TArray<TObjectPtr<ATool>> Tools;

	UPROPERTY(SaveGame)
	TArray<TObjectPtr<ALetter>> QuestItems;
	
	UPROPERTY(SaveGame)
	TObjectPtr<ADrJonesCharacter> Owner;
};