// Property of Night Shift Games, all rights reserved.

#include "EquipmentComponent.h"

#include "EnhancedInputComponent.h"
#include "Animation/CharacterAnimationComponent.h"
#include "Items/Tools/Tool.h"
#include "Player/WidgetManager.h"
#include "Utilities.h"
#include "Items/Letter.h"
#include "SharedComponents/ActionComponent.h"

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
		AddTool(*NewTool);
	}
}

void UEquipmentComponent::SetupPlayerInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(ChangeItemAction, ETriggerEvent::Triggered, this, &UEquipmentComponent::ChangeActiveItem);
	EnhancedInputComponent->BindAction(OpenEquipmentAction, ETriggerEvent::Triggered, this, &UEquipmentComponent::OpenInventory);
	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Triggered, this, &UEquipmentComponent::CallAction);
	EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Triggered, this, &UEquipmentComponent::CallSecondaryAction);

	FInputActionBinding CancelHold(TEXT("CancelItemHold"), IE_Pressed);
	CancelHold.ActionDelegate.GetDelegateForManualSet().BindLambda( [this]()
	{
		DetachActiveItemFromHand();
	});
	EnhancedInputComponent->AddActionBinding(CancelHold);
}

void UEquipmentComponent::AddArtifact(AArtifact& ArtifactToAdd)
{
	SetActiveItem(&ArtifactToAdd);
}

void UEquipmentComponent::AddTool(ATool& ToolToAdd)
{
	Tools.Emplace(&ToolToAdd);
	if (!ItemInHand)
	{
		SetActiveItem(&ToolToAdd);
	}
	else
	{
		AttachItemToHand(ToolToAdd);
	}
	
	Owner->WidgetManager->RequestWidgetUpdate(ItemInfo, NullOpt);
	OnToolAdded.Broadcast(&ToolToAdd);
}

void UEquipmentComponent::RemoveTool(ATool& ToolToRemove)
{
	Tools.Remove(&ToolToRemove);
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
		SetActiveItem(ActiveItem);
	}
}

void UEquipmentComponent::SetActiveItemByClass(TSubclassOf<AItem> ItemClass)
{
	const auto FoundItem = Tools.FindByPredicate([&](const AItem* ItemToCheck) { return ItemToCheck->IsA(ItemClass);});
	SetActiveItem(*FoundItem);
}

void UEquipmentComponent::SetActiveItem(AItem* NewActiveItem)
{
	if (NewActiveItem == ItemInHand)
	{
		return;
	}

	if (ItemInHand && ItemInHand->IsA<ATool>())
	{
		// TODO: Recreating & Destroying mesh
		ItemInHand->FindComponentByClass<UMeshComponent>()->SetVisibility(false);
	}

	ItemInHand = NewActiveItem;
	
	if (NewActiveItem)
	{
		AttachItemToHand(*NewActiveItem);
		ItemInHand->GetMeshComponent()->SetVisibility(true);
	}
	
	Owner->CharacterAnimationComponent->SetActiveItemAnimation(NewActiveItem ? NewActiveItem->GetItemAnimation() : nullptr);
	
	if (UDrJonesWidgetBase* Widget = Utilities::GetWidget(*this, ItemInfo))
	{
		Widget->UpdateData();
	}
}

void UEquipmentComponent::AttachItemToHand(AItem& ItemToAttach)
{
	ItemToAttach.SetupItemInHandProperties();
	ItemToAttach.AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, ItemToAttach.GetItemAttachmentSocket());
}

AItem* UEquipmentComponent::DetachActiveItemFromHand()
{
	if (!ItemInHand)
	{
		return nullptr;;
	}
	
	if (ItemInHand->IsA<AArtifact>() || ItemInHand->IsA<ALetter>())
	{
		ItemInHand->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ItemInHand->SetupItemWorldProperties();

		TArray<AActor*> ActorsToIgnore = {Owner, ItemInHand};
		Algo::Copy(Tools, ActorsToIgnore);
		Algo::Copy(Owner->Children, ActorsToIgnore);

		FVector GroundLocation = Utilities::FindGround(*this, ItemInHand->GetActorLocation(), ActorsToIgnore);
		GroundLocation.Z -= Utilities::GetMeshZOffset(*ItemInHand);
		
		ItemInHand->SetActorLocationAndRotation(GroundLocation, FRotator(0, GetOwner()->GetActorRotation().Yaw,0));
	}
	
	AItem* ReturnValue = ItemInHand;
	SetActiveItem(nullptr);
	
	return ReturnValue;
}

bool UEquipmentComponent::CanPickUpItem() const
{
	return !ItemInHand || ItemInHand && !ItemInHand->IsA<AArtifact>();
}

void UEquipmentComponent::OpenInventory(const FInputActionValue& InputActionValue)
{
	const bool bOpen = InputActionValue.Get<bool>();
	if (!InventoryMenu)
	{
		return;
	}
	UWidgetManager* WidgetManager = Owner->GetWidgetManager();
	UDrJonesWidgetBase* ItemMenu = Utilities::GetWidget(*this, InventoryMenu);
	if (!ItemMenu)
	{
		WidgetManager->AddWidget(InventoryMenu);
		ItemMenu = WidgetManager->GetWidget(InventoryMenu);
	}
	WidgetManager->SetWidgetVisibility(InventoryMenu, bOpen ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	ItemMenu->UpdateData();
}

void UEquipmentComponent::CallAction()
{
	UActionComponent* ReactionComponent = ItemInHand->FindComponentByClass<UActionComponent>();
	ReactionComponent->CallPrimaryAction(GetOwner<ADrJonesCharacter>());
}

void UEquipmentComponent::CallSecondaryAction()
{
	UActionComponent* ReactionComponent = ItemInHand->FindComponentByClass<UActionComponent>();
	ReactionComponent->CallSecondaryAction(GetOwner<ADrJonesCharacter>());
}
