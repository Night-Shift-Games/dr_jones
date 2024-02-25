// Property of Night Shift Games, all rights reserved.

#include "EquipmentComponent.h"

#include "EnhancedInputComponent.h"
#include "Animation/CharacterAnimationComponent.h"
#include "Items/ActionComponent.h"
#include "Items/Artifacts/Artifact.h"
#include "Items/Letter.h"
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
	}
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

	Utilities::GetWidgetManager(*this).RequestWidgetUpdate(ItemInfo);
}

void UEquipmentComponent::EquipItemByClass(TSubclassOf<AItem> ItemClass)
{
	const auto FoundItem = Tools.FindByPredicate([&](const AItem* ItemToCheck) { return ItemToCheck->IsA(ItemClass);});
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
	
	if (UDrJonesWidgetBase* Widget = Utilities::GetWidget(*this, ItemInfo))
	{
		Widget->UpdateData();
	}
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
	
	if (ItemToUnequip->IsA<AArtifact>())
	{
		DetachItemFromHand(*ItemToUnequip);
	}

	Utilities::GetWidgetManager(*this).RequestWidgetUpdate(ItemInfo);
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

void UEquipmentComponent::ChangeActiveItem(const FInputActionValue& InputActionValue)
{
	const float Value = InputActionValue.Get<float>();
	if (Value == 0 || Tools.IsEmpty())
	{
		return;
	}
	if (ItemInHand && !ItemInHand->IsA<ATool>())
	{
		return;
	}
	int32 ActiveItemID;
	Tools.Find(Cast<ATool>(ItemInHand), ActiveItemID);
	if (AItem* ActiveItem = ActiveItemID != INDEX_NONE ? Tools[Utilities::WrapIndexToArray(ActiveItemID + Value, Tools)] : Tools[0])
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
	ItemToAttach.OnAddedToEquipment();
	ItemToAttach.AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, ItemToAttach.GetItemAttachmentSocket());
}

void UEquipmentComponent::DetachItemFromHand(AItem& ItemToDetach)
{
	if (!Owner || !ItemToDetach.IsAttachedTo(Owner))
	{
		return;
	}
	
	if (ATool* ToolToDetach = Cast<ATool>(&ItemToDetach))
	{
		Tools.Remove(ToolToDetach);
	}
	else if (ALetter* LetterToDetach = Cast<ALetter>(&ItemToDetach))
	{
		QuestItems.Remove(LetterToDetach);
	}
	ItemToDetach.SetInstigator(Owner);
	ItemToDetach.OnRemovedFromEquipment();
	ItemToDetach.DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void UEquipmentComponent::OpenEquipmentWheel(const FInputActionValue& InputActionValue)
{
	const bool bOpen = InputActionValue.Get<bool>();
	if (!InventoryMenu)
	{
		return;
	}
	
	Utilities::GetWidgetManager(*this).SetWidgetVisibility(InventoryMenu, bOpen ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	Utilities::GetWidgetManager(*this).RequestWidgetUpdate(InventoryMenu);
}