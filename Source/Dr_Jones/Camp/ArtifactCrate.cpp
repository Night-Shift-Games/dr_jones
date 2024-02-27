#include "ArtifactCrate.h"

#include "Equipment/EquipmentComponent.h"
#include "Interaction/InteractableComponent.h"
#include "Items/Artifacts/Artifact.h"
#include "Reputation/ReputationComponent.h"
#include "UI/ReturnArtifactWidget.h"
#include "UI/WidgetManager.h"
#include "Utilities/Utilities.h"

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
	const UEquipmentComponent* Equipment = Player->GetEquipment();
	AItem* ItemInHand = Equipment->GetItemInHand();
	if (AArtifact* Artifact = ItemInHand ? Cast<AArtifact>(ItemInHand) : nullptr)
	{
		AddArtifact(Artifact, Player);
	}
	
	UWidgetManager& WidgetManager = Utilities::GetWidgetManager(*this);
	WidgetManager.AddWidget(ReturnArtifactsWidgetClass);
	WidgetManager.ShowWidget(ReturnArtifactsWidgetClass);
	UReturnArtifactWidget* Widget = UWidgetManager::GetWidget<UReturnArtifactWidget>(*this, ReturnArtifactsWidgetClass);
	Widget->OwningArtifactCrate = this;
	Widget->OnShow();
	UWidgetManager::UpdateWidget(*this, ReturnArtifactsWidgetClass);
}

void AArtifactCrate::AddArtifact(AArtifact* ArtifactToAdd, ADrJonesCharacter* Player)
{
	Player->GetEquipment()->TakeOutItemInHand();
	if (Artifacts.Contains(ArtifactToAdd))
	{
		return; 
	}
	Artifacts.Add(ArtifactToAdd);
	ArtifactToAdd->AttachToComponent(CrateStaticMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
	ArtifactToAdd->GetMeshComponent()->SetVisibility(true);
	
	UWidgetManager::UpdateWidget(*this, ReturnArtifactsWidgetClass);
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
	UReturnArtifactWidget* Widget = UWidgetManager::GetWidget<UReturnArtifactWidget>(*this, ReturnArtifactsWidgetClass);
	if (!Widget)
	{
		return;
	}
	Utilities::GetWidgetManager(*this).SetWidgetVisibility(ReturnArtifactsWidgetClass, ESlateVisibility::Hidden);
	Widget->OnHide();
}

