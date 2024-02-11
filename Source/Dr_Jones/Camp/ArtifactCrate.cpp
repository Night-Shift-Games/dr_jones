#include "ArtifactCrate.h"

#include "Utilities.h"
#include "Items/Artifacts/Artifact.h"
#include "Player/WidgetManager.h"
#include "Player/PlayerComponents/EquipmentComponent.h"
#include "Player/PlayerComponents/ReputationComponent.h"

AArtifactCrate::AArtifactCrate() : Super()
{
	CrateStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Artifact Crate Static Mesh"));
	RootComponent = CrateStaticMesh;
}

void AArtifactCrate::BeginPlay()
{
	Super::BeginPlay();
	InteractableComponent->InteractDelegate.AddDynamic(this, &AArtifactCrate::OnInteract);
}

void AArtifactCrate::OnInteract(ADrJonesCharacter* Player)
{
	UWidgetManager* WidgetManager = Player->GetWidgetManager();
	WidgetManager->AddWidget(ReturnArtifactsWidgetClass);
	UReturnArtifactWidget* Widget = Cast<UReturnArtifactWidget>(WidgetManager->GetWidget(ReturnArtifactsWidgetClass));
	WidgetManager->ShowWidget(ReturnArtifactsWidgetClass);

	UEquipmentComponent* Inventory = Player->GetInventory();
	AItem* ItemInHand = Inventory->GetItemInHand();
	if (AArtifact* Artifact = ItemInHand ? Cast<AArtifact>(ItemInHand) : nullptr)
	{
		Inventory->DetachActiveItemFromHand();
		AddArtifact(Artifact, Player);
	}

	Widget->OwningArtifactCrate = this;
	Widget->OnShow();
	Widget->UpdateData();
}

void AArtifactCrate::AddArtifact(AArtifact* ArtifactToAdd, ADrJonesCharacter* Player)
{
	Player->GetInventory()->DetachActiveItemFromHand();
	if (Artifacts.Contains(ArtifactToAdd))
	{
		return; 
	}
	Artifacts.Add(ArtifactToAdd);
	ArtifactToAdd->AttachToComponent(CrateStaticMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
	ArtifactToAdd->GetMeshComponent()->SetVisibility(true);

	if (UReturnArtifactWidget* Widget = Utilities::GetWidget<UReturnArtifactWidget>(*this, ReturnArtifactsWidgetClass))
	{
		Widget->UpdateData();
	}
}

AArtifact* AArtifactCrate::PullOutArtifact(AArtifact* ArtifactToPullOut)
{
	if (!Artifacts.Contains(ArtifactToPullOut))
	{
		return nullptr; 
	}
	Artifacts.Remove(ArtifactToPullOut);
	return ArtifactToPullOut;
}

void AArtifactCrate::SendArtifacts()
{
	while (!Artifacts.IsEmpty())
	{
		AArtifact* ArtifactToSend = Artifacts.Last();
		PullOutArtifact(ArtifactToSend);
		ArtifactToSend->Destroy();
		// TODO: Calculate renown based on artifact parameters.
		const float RenownToAdd = ArtifactToSend->bArtifactCleared ? 20.f : 10.f;
		Utilities::GetPlayerCharacter(*this).ReputationComponent->AddReputation(IsArchaeologistCrate ? EReputationType::Archaeologist : EReputationType::TreasureHunter, RenownToAdd);
	}
}

void AArtifactCrate::CloseWidget()
{
	UWidgetManager* WidgetManager = Utilities::GetPlayerCharacter(*this).GetWidgetManager();
	UReturnArtifactWidget* Widget = Cast<UReturnArtifactWidget>(WidgetManager->GetWidget(ReturnArtifactsWidgetClass));
	if (!Widget)
	{
		return;
	}
	Widget->OnHide();
	WidgetManager->HideWidget(ReturnArtifactsWidgetClass);
}

