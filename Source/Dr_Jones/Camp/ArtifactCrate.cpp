#include "ArtifactCrate.h"

#include "Utilities.h"
#include "Items/Artifacts/Artifact.h"
#include "Player/WidgetManager.h"
#include "Player/PlayerComponents/InventoryComponent.h"
#include "Player/PlayerComponents/ReputationComponent.h"

AArtifactCrate::AArtifactCrate()
{
	CrateStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Artifact Crate Static Mesh"));
	RootComponent = CrateStaticMesh;
	PrimaryActorTick.bCanEverTick = false;
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable Component"));
	InteractableComponent->InteractDelegate.AddDynamic(this, &AArtifactCrate::OnInteract);
}

void AArtifactCrate::BeginPlay()
{
	Super::BeginPlay();
}

void AArtifactCrate::OnInteract(ADrJonesCharacter* Player)
{
	UWidgetManager* WidgetManager = Player->GetWidgetManager();
	WidgetManager->AddWidget(ReturnArtifactsWidgetClass);
	UReturnArtifactWidget* Widget = Cast<UReturnArtifactWidget>(WidgetManager->GetWidget(ReturnArtifactsWidgetClass));
	WidgetManager->ShowWidget(ReturnArtifactsWidgetClass);
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
	
	UReturnArtifactWidget* Widget = Utilities::GetWidget<UReturnArtifactWidget>(*this, ReturnArtifactsWidgetClass);
	Widget->UpdateData();
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
	for (int i = 0; i < Artifacts.Num(); i++)
	{
		AArtifact* ArtifactToRemove = PullOutArtifact(Artifacts[i]);
		ArtifactToRemove->Destroy();
		Utilities::GetPlayerCharacter(*this).ReputationComponent->AddReputation(IsArchaeologistCrate ? EReputationType::Archaeologist : EReputationType::TreasureHunter, 10.f);
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

