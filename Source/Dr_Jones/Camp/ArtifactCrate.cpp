#include "ArtifactCrate.h"

#include "Equipment/EquipmentComponent.h"
#include "Interaction/InteractableComponent.h"
#include "Items/Artifacts/Artifact.h"
#include "Reputation/ReputationComponent.h"
#include "UI/ReturnArtifactWidget.h"
#include "UI/WidgetManager.h"
#include "UI/WidgetRendererComponent.h"
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
	UEquipmentComponent* Equipment = Player->GetEquipment();
	AItem* ItemInHand = Equipment->GetItemInHand();
	if (AArtifact* Artifact = ItemInHand ? Cast<AArtifact>(ItemInHand) : nullptr)
	{
		Equipment->TakeOutItemInHand();
		AddArtifact(Artifact);
	}
}

void AArtifactCrate::AddArtifact(AArtifact* ArtifactToAdd)
{
	NS_EARLY(Artifacts.Contains(ArtifactToAdd));
	
	Artifacts.Add(ArtifactToAdd);
	ArtifactToAdd->AttachToComponent(CrateStaticMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
	ArtifactToAdd->GetMeshComponent()->SetVisibility(false);
	ArtifactToAdd->SetActorEnableCollision(false);

	UWidgetRendererComponent* WidgetRenderer = FindComponentByClass<UWidgetRendererComponent>();
	NS_EARLY(!WidgetRenderer || !WidgetRenderer->GetInternalWidget());
	UReturnArtifactWidget* ReturnArtifactWidget = Cast<UReturnArtifactWidget>(WidgetRenderer->GetInternalWidget());
	ReturnArtifactWidget->OwningArtifactCrate = this;
	ReturnArtifactWidget->UpdateData();
}

AArtifact* AArtifactCrate::PullOutArtifact(AArtifact* ArtifactToPullOut)
{
	NS_EARLY_R (!Artifacts.Contains(ArtifactToPullOut), nullptr);
	
	Artifacts.Remove(ArtifactToPullOut);
	ArtifactToPullOut->SetActorEnableCollision(false);
	ArtifactToPullOut->GetMeshComponent()->SetVisibility(true);

	UWidgetManager::UpdateWidget(*this, ReturnArtifactsWidgetClass);
	
	return ArtifactToPullOut;
}

void AArtifactCrate::SendArtifacts(int32& OutCalculatedReputation)
{
	float RenownToAdd = 0.0f;
	while (!Artifacts.IsEmpty())
	{
		AArtifact* ArtifactToSend = Artifacts.Last();
		PullOutArtifact(ArtifactToSend);
		// TODO: Calculate renown based on artifact parameters.
		RenownToAdd += 10.0f;
		RenownToAdd += ArtifactToSend->CleaningProgress * 10.0f;
		ArtifactToSend->Destroy();
	}
	if (bAffectReputation)
	{
		Utilities::GetPlayerCharacter(*this).ReputationComponent->AddReputation(IsArchaeologistCrate ? EReputationType::Archaeologist : EReputationType::TreasureHunter, RenownToAdd);
	}
	OutCalculatedReputation = RenownToAdd;
	UWidgetManager::UpdateWidget(*this, ReturnArtifactsWidgetClass);
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

