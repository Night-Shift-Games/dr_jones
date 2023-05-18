// Property of Night Shift Games, all rights reserved.

#include "Items/Tools/Tool.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Player/DrJonesCharacter.h"
#include "Player/PlayerComponents/HotBarComponent.h"
#include "SharedComponents/ActionComponent.h"
#include "SharedComponents/InteractableComponent.h"

ATool::ATool()
{
	ActionComponent = CreateDefaultSubobject<UActionComponent>(TEXT("ActionComponent"));
}

void ATool::BeginPlay()
{
	Super::BeginPlay();
	InteractableComponent->InteractDelegate.AddUniqueDynamic(this, &ATool::PickUp);
}

void ATool::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	InteractableComponent->InteractDelegate.RemoveDynamic(this, &ATool::PickUp);
}

void ATool::PickUp(ADrJonesCharacter* Player)
{
	checkf(Player, TEXT("Player is missing!"));
	OwningPlayer = Player;
	
	GetRootComponent()->SetMobility(EComponentMobility::Movable);
	if (UMeshComponent* Mesh = GetMeshComponent())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}

	checkf(Player->HotBarComponent, TEXT("Toolbar component is missing!"));
	Player->HotBarComponent->AddTool(*this);
	// TODO: Attaching & reattaching should be inside Hotbar.
	Player->HotBarComponent->SetActiveItem(*this);
	AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachmentSocket);
}

UAnimMontage* ATool::FindActionMontage(const FName& MontageName) const
{
	if (!OwningPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot play tool montage because the tool is not owned by any player."));
		return nullptr;
	}

	checkf(OwningPlayer->CharacterAnimationComponent, TEXT("Animation Component is null"));
	return OwningPlayer->CharacterAnimationComponent->FindItemActionMontage(*this, MontageName);
}
