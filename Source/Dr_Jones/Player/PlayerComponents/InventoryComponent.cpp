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
		// TODO: XDDDDD
		NewTool->PickUp(Owner.Get());
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
	InputComponent->AddActionBinding(OpenInventoryMenu);
	InputComponent->AddActionBinding(CloseInventoryMenu);
	InputComponent->BindAction(TEXT("CancelItemHold"), IE_Pressed, this, &UInventoryComponent::DetachActiveItemFromHand);
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
	if (Tools.Find(ActiveTool, ActiveItemID))
	{
		SetActiveItem(*Tools[Utilities::WrapIndexToArray(ActiveItemID + Value, Tools)]);
	}
}

void UInventoryComponent::AddTool(ATool& ToolToAdd)
{
	Tools.Emplace(&ToolToAdd);
	if (Tools.Num() < 2)
	{
		SetActiveItem(ToolToAdd);
	}
	const TOptional<float> None;
	Owner->WidgetManager->RequestWidgetUpdate(ItemInfo, None);

	OnToolAdded.Broadcast(&ToolToAdd);
}

void UInventoryComponent::RemoveTool(ATool& ToolToRemove)
{
	Tools.Remove(&ToolToRemove);
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

	ItemInHand = &NewActiveItem;

	if (NewActiveItem.IsA<ATool>())
	{
		ActiveTool = Cast<ATool>(&NewActiveItem);
	}
	else if (ActiveTool)
	{
		ActiveTool->GetMeshComponent()->SetVisibility(false);
	}
	
	NewActiveItem.GetMeshComponent()->SetVisibility(true);
	Owner->ReactionComponent->SetActiveItem(NewActiveItem);
	Owner->CharacterAnimationComponent->SetActiveItemAnimation(NewActiveItem.GetItemAnimation());
	Owner->GetWidgetManager()->RequestWidgetUpdate(ItemInfo, NullOpt);
}

void UInventoryComponent::AttachItemToHand(AItem& ItemToAttach)
{
	ItemToAttach.AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("RightHandSocket"));
}

void UInventoryComponent::DetachActiveItemFromHand()
{
	if (!ItemInHand)
	{
		return;
	}
	ItemInHand->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemInHand->GetMeshComponent()->SetSimulatePhysics(true);
	ItemInHand->EnableInteraction(true);
	ItemInHand = nullptr;
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

TArray<ATool*> UInventoryComponent::GetTools() const
{
	return Tools;
}

void UInventoryComponent::AddArtifact(AArtifact& Artifact)
{
	ArtifactInHand = &Artifact;
}
