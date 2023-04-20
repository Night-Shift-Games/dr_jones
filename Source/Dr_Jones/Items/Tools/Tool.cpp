// Property of Night Shift Games, all rights reserved.

#include "Items/Tools/Tool.h"

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

	GetRootComponent()->SetMobility(EComponentMobility::Movable);
	
	SetActorEnableCollision(false);
	
	if (!Player->ToolbarComponent)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10, FColor::Red, TEXT("Cursed array"));
		return;
	}
	Player->ToolbarComponent->AddTool(this);
	Player->ToolbarComponent->SetActiveItem(*this);

	AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("RightHandSocket"));

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10, FColor::Red, TEXT("Test"));
}
