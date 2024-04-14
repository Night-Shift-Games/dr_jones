// Property of Night Shift Games, all rights reserved.

#include "Sieve.h"

#include "Animation/DrJonesAnimNotify.h"
#include "Interaction/InteractableComponent.h"
#include "Items/ActionComponent.h"
#include "Utilities/Utilities.h"

// Called when the game starts or when spawned
void ASieve::BeginPlay()
{
	Super::BeginPlay();
	ActionComponent->PrimaryActionDelegate.AddDynamic(this, &ASieve::Shake);
}

void ASieve::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASieve::Shake()
{
	UDrJonesAnimNotify* DrJonesAnimNotify = nullptr;
	ShakeMontage->Notifies.FindByPredicate([&](const FAnimNotifyEvent& Event)
	{
		DrJonesAnimNotify = Cast<UDrJonesAnimNotify>(Event.Notify);
		return IsValid(DrJonesAnimNotify);
	});
	if (!DrJonesAnimNotify)
	{
		return;
	}
	DrJonesAnimNotify->OnNotifyHit.AddWeakLambda(this, [&, DrJonesAnimNotify]()
	{
		SpawnArtifacts();
		DrJonesAnimNotify->OnNotifyHit.RemoveAll(this);
	});
	Utilities::GetPlayerCharacter(*this).PlayAnimMontage(ShakeMontage);
}

void ASieve::SpawnArtifacts()
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, __func__);
}

