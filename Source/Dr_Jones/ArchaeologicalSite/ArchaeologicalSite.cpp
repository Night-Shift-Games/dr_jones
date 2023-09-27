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

void AArchaeologicalSite::ModifyTerrainAt(FTransform ModifyLocation)
{
	if (!Terrain)
	{
		Terrain = NewObject<UTerrain>();
		Terrain->OwningArchaeologicalSite = this;
	}
	Terrain->ModifyAt(ModifyLocation, FTransform::Identity);
}

void UTerrain::ModifyAt(const FTransform& Location, const FTransform& Shape)
{
	CreateSectionAt(Location.GetLocation());
	UExcavationSegment* Segment = Segments.FindRef(Location.GetLocation());
	if (!Segment)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Coś zjebałeś z lokacją"));
		return;
	}
	Segment->Dig(Location, FVector(50,50,50));
}

void UTerrain::CreateSectionAt(const FVector& Location)
{
	if (Segments.Find(Location))
	{
		return;
	}
	
	UExcavationSegment* Segment = NewObject<UExcavationSegment>(OwningArchaeologicalSite);
	Segment->AttachToComponent(OwningArchaeologicalSite->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	Segment->RegisterComponent();
	Segment->SetComponentToWorld(FTransform(Location));
	Segment->GenerateMesh(100, 10000.f);

	Segments.Emplace(Location, Segment);
}
