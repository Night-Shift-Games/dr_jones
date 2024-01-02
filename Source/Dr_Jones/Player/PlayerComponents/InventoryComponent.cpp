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
	if (Tools.Num() < 2)
	{
		SetActiveItem(ToolToAdd);
	}
	else
	{
		AttachItemToHand(ToolToAdd);
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
	
	if (UDrJonesWidgetBase* Widget = Utilities::GetWidget(*this, ItemInfo))
	{
		Widget->UpdateData();
	}
}

void UInventoryComponent::AttachItemToHand(AItem& ItemToAttach)
{
	ItemToAttach.SetupItemInHandProperties();
	ItemToAttach.AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, ItemToAttach.GetItemAttachmentSocket());
}

AItem* UInventoryComponent::DetachActiveItemFromHand()
{
	if (!ItemInHand || ItemInHand->IsA<ATool>())
	{
		return nullptr;;
	}

	ItemInHand->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemInHand->SetupItemGroundProperties();
	TArray<AActor*> ActorsToIgnore = {Owner, ItemInHand};
	Algo::Copy(Tools, ActorsToIgnore);
	Algo::Copy(Owner->Children, ActorsToIgnore);
	const FVector GroundLocation = Utilities::FindGround(*this, ItemInHand->GetActorLocation(), ActorsToIgnore);
	ItemInHand->SetActorLocationAndRotation(GroundLocation, FRotator::ZeroRotator);
	AItem* ReturnValue = ItemInHand;
	ItemInHand = nullptr;
	return ReturnValue;
}

bool UInventoryComponent::CanPickUpItem() const
{
	return !ItemInHand || ItemInHand && !ItemInHand->IsA<AArtifact>();
}

void UInventoryComponent::OpenInventory(bool bOpen) const
{
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

