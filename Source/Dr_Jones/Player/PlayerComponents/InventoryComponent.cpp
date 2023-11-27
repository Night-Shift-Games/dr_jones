// Property of Night Shift Games, all rights reserved.

#include "InventoryComponent.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Items/Tools/Tool.h"
#include "Player/WidgetManager.h"
#include "ReactionComponent.h"
#include "Utilities.h"

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner<ADrJonesCharacter>();
	UWorld* World = GetWorld();
	check (Owner.Get())
	check (World)
	for (TSubclassOf<ATool> ToolClass : DefaultTools)
	{
		ATool* NewTool = World->SpawnActor<ATool>(ToolClass);
		AddTool(*NewTool);
	}
}

void UInventoryComponent::SetupPlayerInput(UInputComponent* InputComponent)
{
	InputComponent->BindAxis(TEXT("Scroll"), this, &UInventoryComponent::ChangeActiveItem);
	FInputActionBinding OpenInventoryMenu(TEXT("Inventory"), IE_Pressed);
	OpenInventoryMenu.ActionDelegate.GetDelegateForManualSet().BindLambda( [this]()
	{
		OpenInventory(true);
	});
	FInputActionBinding CloseInventoryMenu(TEXT("Inventory"), IE_Released);
	CloseInventoryMenu.ActionDelegate.GetDelegateForManualSet().BindLambda( [this]()
	{
		OpenInventory(false);
	});
	FInputActionBinding CancelHold(TEXT("CancelItemHold"), IE_Pressed);
	CancelHold.ActionDelegate.GetDelegateForManualSet().BindLambda( [this]()
	{
		DetachActiveItemFromHand();
	});
	InputComponent->AddActionBinding(OpenInventoryMenu);
	InputComponent->AddActionBinding(CloseInventoryMenu);
	InputComponent->AddActionBinding(CancelHold);
}

void UInventoryComponent::AddArtifact(AArtifact& ArtifactToAdd)
{
	SetActiveItem(ArtifactToAdd);
}

void UInventoryComponent::AddTool(ATool& ToolToAdd)
{
	Tools.Emplace(&ToolToAdd);
	AttachItemToHand(ToolToAdd);

	if (Tools.Num() < 2)
	{
		SetActiveItem(ToolToAdd);
	}
	
	Owner->WidgetManager->RequestWidgetUpdate(ItemInfo, NullOpt);
	OnToolAdded.Broadcast(&ToolToAdd);
}

void UInventoryComponent::RemoveTool(ATool& ToolToRemove)
{
	Tools.Remove(&ToolToRemove);
}

void UInventoryComponent::ChangeActiveItem(float Value)
{
	if (Value == 0 || Tools.Num() < 2)
	{
		return;
	}
	if (ItemInHand && ItemInHand->IsA<AArtifact>())
	{
		return;
	}
	int32 ActiveItemID;
	Tools.Find(Cast<ATool>(ItemInHand), ActiveItemID);
	if (AItem* ActiveItem = ActiveItemID != INDEX_NONE ? Tools[Utilities::WrapIndexToArray(ActiveItemID + Value, Tools)] : Tools[0])
	{
		SetActiveItem(*ActiveItem);
	}
}

void UInventoryComponent::SetActiveItem(AItem& NewActiveItem)
{
	if (&NewActiveItem == ItemInHand)
	{
		return;
	}
	
	if (ItemInHand)
	{
		// TODO: Recreating & Destroying mesh
		ItemInHand->FindComponentByClass<UMeshComponent>()->SetVisibility(false);
	}

	AttachItemToHand(NewActiveItem);
	ItemInHand = &NewActiveItem;
	
	ItemInHand->GetMeshComponent()->SetVisibility(true);
	Owner->ReactionComponent->SetActiveItem(NewActiveItem);
	Owner->CharacterAnimationComponent->SetActiveItemAnimation(NewActiveItem.GetItemAnimation());

	Owner->GetWidgetManager()->RequestWidgetUpdate(ItemInfo, NullOpt);
}

void UInventoryComponent::AttachItemToHand(AItem& ItemToAttach)
{
	ItemToAttach.SetupItemInHandProperties();
	ItemToAttach.AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, ItemToAttach.GetItemAttachmentSocket());
	ItemInHand = &ItemToAttach;
}

AItem* UInventoryComponent::DetachActiveItemFromHand()
{
	if (!ItemInHand || ItemInHand->IsA<ATool>())
	{
		return nullptr;;
	}

	ItemInHand->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemInHand->SetupItemGroundProperties();
	AItem* ReturnValue = ItemInHand;
	ItemInHand = nullptr;
	return ReturnValue;
}

void UInventoryComponent::OpenInventory(bool bOpen) const
{
	if (!InventoryMenu)
	{
		return;
	}
	UWidgetManager* WidgetManager = Owner->GetWidgetManager();
	UDrJonesWidgetBase* ItemMenu = WidgetManager->GetWidget(InventoryMenu);
	if (!ItemMenu)
	{
		WidgetManager->AddWidget(InventoryMenu);
		ItemMenu = WidgetManager->GetWidget(InventoryMenu);
	}
	if (bOpen && !ItemMenu->IsInViewport())
	{
		WidgetManager->ShowWidget(InventoryMenu);
	}
	else if (!bOpen && ItemMenu->IsInViewport())
	{
		WidgetManager->HideWidget(InventoryMenu);
	}
	ItemMenu->UpdateData();
}

