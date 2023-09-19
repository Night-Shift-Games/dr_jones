// Property of Night Shift Games, all rights reserved.

#include "ArchaeologicalSite.h"

AArchaeologicalSite::AArchaeologicalSite()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AArchaeologicalSite::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}