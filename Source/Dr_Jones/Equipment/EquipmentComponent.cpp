// Property of Night Shift Games, all rights reserved.

#include "EquipmentComponent.h"

#include "EnhancedInputComponent.h"
#include "EquipmentWidget.h"
#include "Animation/CharacterAnimationComponent.h"
#include "Items/ActionComponent.h"
#include "Items/Artifacts/Artifact.h"
#include "Items/Letter.h"
#include "Items/Tools/Bucket.h"
#include "Items/Tools/Tool.h"
#include "UI/WidgetManager.h"
#include "Utilities/Utilities.h"

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner<ADrJonesCharacter>();
	UWorld* World = GetWorld();
	check (Owner)
	check (World)
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Owner;
	for (TSubclassOf<ATool> ToolClass : DefaultTools)
	{
		ATool* NewTool = World->SpawnActor<ATool>(ToolClass, SpawnParameters);
		NewTool->GetMeshComponent()->SetVisibility(false);
		AddItem(NewTool);
		if (Tools.Num() < 3)
		{
			AddToQuickSlot(NewTool->GetClass(), Tools.Num() - 1);
		}
	}
	
	UWidgetManager::RequestUpdateWidget<UEquipmentWidgetDataObject>(*this, InventoryMenu, [&](UEquipmentWidgetDataObject& DataObject)
	{
		DataObject.UpdatingEquipment = this;
	});
}

void UEquipmentComponent::SetupPlayerInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(ChangeItemAction, ETriggerEvent::Triggered, this, &UEquipmentComponent::ChangeActiveItem);
	EnhancedInputComponent->BindAction(OpenEquipmentAction, ETriggerEvent::Triggered, this, &UEquipmentComponent::OpenEquipmentWheel);
	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Triggered, this, &UEquipmentComponent::CallPrimaryItemAction);
	EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Triggered, this, &UEquipmentComponent::CallSecondaryItemAction);
	EnhancedInputComponent->BindAction(UnequipItemAction, ETriggerEvent::Triggered, this, &UEquipmentComponent::UnequipItem);
}

void UEquipmentComponent::AddItem(AItem* ItemToAdd)
{
	if (!ItemToAdd)
	{
		return;
	}

	if (ATool* Tool = Cast<ATool>(ItemToAdd))
	{
		Tools.Emplace(Tool);
		OnToolAdded.Broadcast(Tool);
	}
	else if (ALetter* LetterToAdd = Cast<ALetter>(ItemToAdd))
	{
		QuestItems.Emplace(LetterToAdd);
	}
	
	AttachItemToHand(*ItemToAdd);
	EquipItem(ItemToAdd);
}

void UEquipmentComponent::EquipItemByClass(const TSubclassOf<AItem> ItemClass)
{
	TArray<AItem*> Items;
	Algo::Copy(Tools, Items);
	Algo::Copy(QuestItems, Items);
	const auto FoundItem = Items.FindByPredicate([&](const AItem* ItemToCheck) { return ItemToCheck->IsA(ItemClass);});
	if (!FoundItem)
	{
		return;
	}
	EquipItem(*FoundItem);
}

void UEquipmentComponent::EquipItem(AItem* NewActiveItem)
{
	if (NewActiveItem == ItemInHand)
	{
		return;
	}
	
	UnequipItem();

	ItemInHand = NewActiveItem;
	
	if (ItemInHand)
	{
		ItemInHand->OnEquip();
	}
	
	Owner->CharacterAnimationComponent->SetActiveItemAnimation(ItemInHand ? ItemInHand->GetItemAnimation() : nullptr);

	UWidgetManager::UpdateWidget(*this, ItemInfo);
}

void UEquipmentComponent::UnequipItem()
{
	if (!ItemInHand)
	{
		return;
	}
	AItem* ItemToUnequip = ItemInHand;
	ItemInHand = nullptr;
	
	ItemToUnequip->OnUnequip();
	
	if (ItemToUnequip->IsA<AArtifact>() || ItemToUnequip->IsA<ABucket>())
	{
		DetachItemFromHand(*ItemToUnequip);
	}

	UWidgetManager::UpdateWidget(*this, ItemInfo);
}

AItem* UEquipmentComponent::TakeOutItemInHand()
{
	if (!ItemInHand)
	{
		return nullptr;
	}

	AItem* ItemToDetach = ItemInHand;
	UnequipItem();
	DetachItemFromHand(*ItemToDetach);
	
	return ItemToDetach;
}

bool UEquipmentComponent::CanPickUpItem() const
{
	return !ItemInHand || ItemInHand && !ItemInHand->IsA<AArtifact>();
}

void UEquipmentComponent::CallPrimaryItemAction()
{
	if (!ItemInHand)
	{
		return;
	}
	UActionComponent* ReactionComponent = ItemInHand->FindComponentByClass<UActionComponent>();
	if (!ReactionComponent)
	{
		return;
	}
	ReactionComponent->CallPrimaryAction(GetOwner<ADrJonesCharacter>());
}

void UEquipmentComponent::CallSecondaryItemAction()
{
	if (!ItemInHand)
	{
		return;
	}
	UActionComponent* ReactionComponent = ItemInHand->FindComponentByClass<UActionComponent>();
	if (!ReactionComponent)
	{
		return;
	}
	ReactionComponent->CallSecondaryAction(GetOwner<ADrJonesCharacter>());
}

void UEquipmentComponent::AddToQuickSlot(TSubclassOf<AItem> ItemClass, int Index)
{
	if (AItem* ItemToRemove = QuickSlotItems.IsValidIndex(Index) ? QuickSlotItems[Index] : nullptr)
	{
		RemoveFromQuickSlot(*ItemToRemove);
		if (ItemToRemove->GetClass() == ItemClass)
		{
			return;
		}
	}
	TArray<AItem*> Items;
	Algo::Copy(Tools, Items);
	Algo::Copy(QuestItems, Items);
	const auto FoundItem = Items.FindByPredicate([&](const AItem* ItemToCheck) { return ItemToCheck->IsA(ItemClass);});
	if (!FoundItem)
	{
		return;
	}
	QuickSlotItems.Reserve(3);
	if (QuickSlotItems.IsValidIndex(Index))
	{
		QuickSlotItems.Insert(*FoundItem, Index);
	}
	else
	{
		QuickSlotItems.Add(*FoundItem);
	}

	UWidgetManager::RequestUpdateWidget<UEquipmentWidgetDataObject>(*this, InventoryMenu, [&](UEquipmentWidgetDataObject& Data)
	{
		Data.QuickSlotsItems = &QuickSlotItems;
	});
}

void UEquipmentComponent::RemoveFromQuickSlot(AItem& ItemToRemove)
{
	int OutId;
	if (!QuickSlotItems.Find(&ItemToRemove, OutId))
	{
		return;
	}
	QuickSlotItems.Remove(&ItemToRemove);
}

void UEquipmentComponent::ChangeActiveItem(const FInputActionValue& InputActionValue)
{
	const float Value = InputActionValue.Get<float>();
	if (Value == 0 || QuickSlotItems.IsEmpty())
	{
		return;
	}
	if (ItemInHand && ItemInHand->IsA<AArtifact>())
	{
		return;
	}
	int32 ActiveItemID;
	QuickSlotItems.Find(ItemInHand, ActiveItemID);
	if (AItem* ActiveItem = ActiveItemID != INDEX_NONE ? QuickSlotItems[Utilities::WrapIndexToArray(ActiveItemID + Value, QuickSlotItems)] : QuickSlotItems[0])
	{
		EquipItem(ActiveItem);
	}
}

void UEquipmentComponent::AttachItemToHand(AItem& ItemToAttach)
{
	if (ItemToAttach.IsAttachedTo(Owner))
	{
		return;
	}

	USceneComponent* ItemRootComponent = ItemToAttach.GetRootComponent();
	ItemRootComponent->SetMobility(EComponentMobility::Movable);
	TInlineComponentArray<UMeshComponent*> Components;
	ItemToAttach.GetComponents<UMeshComponent>(Components);
	for (UMeshComponent* Mesh : Components)
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		Mesh->SetSimulatePhysics(false);
	}
	
	if (Owner->GetMovementBase() == ItemToAttach.GetMeshComponent())
	{
		Owner->SetBase(nullptr);
	}
	
	ItemToAttach.AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemToAttach.GetItemAttachmentSocket());

	ItemToAttach.OnAddedToEquipment();
}

void UEquipmentComponent::DetachItemFromHand(AItem& ItemToDetach)
{
	if (!Owner || !ItemToDetach.IsAttachedTo(Owner))
	{
		return;
	}

	RemoveFromQuickSlot(ItemToDetach);
	if (ATool* ToolToDetach = Cast<ATool>(&ItemToDetach))
	{
		Tools.Remove(ToolToDetach);
	}
	else if (ALetter* LetterToDetach = Cast<ALetter>(&ItemToDetach))
	{
		QuestItems.Remove(LetterToDetach);
	}
	ItemToDetach.SetInstigator(Owner);
	
	ItemToDetach.SetWorldPhysics();
	ItemToDetach.DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	ItemToDetach.OnRemovedFromEquipment();
}

void UEquipmentComponent::OpenEquipmentWheel(const FInputActionValue& InputActionValue)
{
	const bool bOpen = InputActionValue.Get<bool>();
	if (!InventoryMenu)
	{
		return;
	}
	
	Utilities::GetWidgetManager(*this).SetWidgetVisibility(InventoryMenu, bOpen ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	UWidgetManager::RequestUpdateWidget<UEquipmentWidgetDataObject>(*this, InventoryMenu, [&](UEquipmentWidgetDataObject& DataObject)
	{
		DataObject.Letters = &QuestItems;
		DataObject.Tools = &Tools;
		DataObject.QuickSlotsItems = &QuickSlotItems;
		DataObject.UpdatingEquipment = this;
	});
}