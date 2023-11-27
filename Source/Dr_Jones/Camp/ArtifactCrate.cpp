#include "ArtifactCrate.h"

#include "Items/Artifacts/Artifact.h"
#include "Kismet/GameplayStatics.h"
#include "Player/WidgetManager.h"
#include "Player/PlayerComponents/InventoryComponent.h"

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

	UWidgetManager* WidgetManager = Player->GetWidgetManager();
	UReturnArtifactWidget* Widget = Cast<UReturnArtifactWidget>(WidgetManager->GetWidget(ReturnArtifactsWidgetClass));
	Widget->UpdateData();
}

AArtifact* AArtifactCrate::PullOutArtifact(AArtifact* ArtifactToPullOut)
{
	if (!Artifacts.Contains(ArtifactToPullOut))
	{
		return nullptr; 
	}
	Artifacts.Remove(ArtifactToPullOut);
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	Cast<ADrJonesCharacter>(PlayerCharacter)->InventoryComponent->AddArtifact(*ArtifactToPullOut);
	return ArtifactToPullOut;
}

void AArtifactCrate::SendArtifacts()
{
	for (AArtifact* Artifact : Artifacts)
	{
		Artifact->Destroy();
	}
}

void AArtifactCrate::CloseWidget()
{
	UWidgetManager* WidgetManager = Cast<ADrJonesCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))->GetWidgetManager();
	UReturnArtifactWidget* Widget = Cast<UReturnArtifactWidget>(WidgetManager->GetWidget(ReturnArtifactsWidgetClass));
	if (!Widget)
	{
		return;
	}
	Widget->OnHide();
	WidgetManager->HideWidget(ReturnArtifactsWidgetClass);
}

