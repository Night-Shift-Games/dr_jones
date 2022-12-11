// Property of Night Shift Games, all rights reserved.


#include "ExcavationArea.h"

AExcavationArea::AExcavationArea() 
{ 
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;
	PrimaryActorTick.bCanEverTick = false; 
	Resolution = 10;
	Size = 1000;
	AreaResolution = 10;
	bIsArenaGenerated = false;
	bSmoothDigging = false;
}

void AExcavationArea::PopulateWithArtefacts()
{
	SpawnAllQuestArtefacts();
	for (size_t i = 0; i < ArtefactsQuantity; i++)
	{
		SpawnArtefact();
	}
}

void AExcavationArea::SpawnArtefact(TSubclassOf<UArtefact>ArtefactClass)
{
	if (ArtefactsClass.IsEmpty())
	{
		return;
	}

	if (!ArtefactClass)
	{
		ArtefactClass = ArtefactsClass[FMath::RandRange(0, ArtefactsClass.Num() - 1)];
	}

	UArtefact* NewArtefact = NewObject<UArtefact>(this, ArtefactClass);
	NewArtefact->RegisterComponent();
	NewArtefact->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	float DigDeep;
	switch (NewArtefact->Rarity)
	{
		case FArtefactRarity::Quest: {}
		case FArtefactRarity::Lore:
		{
			ArtefactsClass.Remove(ArtefactClass);
			DigDeep = 40;
			break;
		}
		default:
		{
			switch (NewArtefact->Size)
			{
				case FArtefactSize::Huge:
				{
					DigDeep = 120;
					break;
				}
				case FArtefactSize::Large:
				{
					DigDeep = 100;
					break;
				}
				case FArtefactSize::Medium:
				{
					DigDeep = 50;
					break;
				}
				default:
				{
					DigDeep = 30;
					break;
				}
			}
		}
	}

	FVector ArtefactLocation = UKismetMathLibrary::RandomPointInBoundingBox(ArtefactSpawnOrigin + FVector(0,0,-DigDeep), FVector(ArtefactSpawnAreaX, ArtefactSpawnAreaY, 20));
	NewArtefact->SetRelativeLocationAndRotation(ArtefactLocation, FRotator(FMath::RandRange(0, 360), FMath::RandRange(0, 360), FMath::RandRange(0, 360)));
	Artefacts.Add(NewArtefact);
}

void AExcavationArea::SpawnAllQuestArtefacts()
{
	TArray <TSubclassOf<UArtefact>> ArtefactsClassCopy = ArtefactsClass;
	if (ArtefactsClass.IsEmpty())
	{
		return;
	}
	for (auto& ArtefactClass : ArtefactsClassCopy)
	{
		UArtefact* Artefact = Cast<UArtefact>(ArtefactClass->GetDefaultObject());
		if (Artefact->Rarity == Lore || Artefact->Rarity == Quest)
		{
			SpawnArtefact(ArtefactClass);
		}
	}

}

void AExcavationArea::BeginPlay()
{
	Super::BeginPlay();
	CreateArea();
	PopulateWithArtefacts();
}

//void AExcavationArea::OnConstruction(const FTransform& Transform)
//{
//	if (bIsArenaGenerated)
//	{
//		RefreshArena();
//	}
//}

void AExcavationArea::CreateArea()
{
	DestroyArea();

	SegmentSize = Size / AreaResolution;
	int i = 0;
	for (int y = 0; y < AreaResolution; y++)
	{
		for (int x = 0; x < AreaResolution; x++)
		{
			FTransform SegmentLocation(FVector(SegmentSize * x, SegmentSize * y, 0) + FVector(-Size / 2.f + SegmentSize / 2, -Size / 2.f + SegmentSize / 2, 0));
			UExcavationSegment* NewSegment = Cast<UExcavationSegment>(AddComponentByClass(UExcavationSegment::StaticClass(), false, SegmentLocation, false));
			ExcavationSegments.Add(NewSegment);
			NewSegment->Material = ExcavateMaterial;
			NewSegment->bSmoothDig = bSmoothDigging;
			NewSegment->GenerateMesh(Resolution, SegmentSize);
	
			if (x != 0)
			{
				int lastNeighbor = i - 1;
				NewSegment->Neighbors.Add(ExcavationSegments[lastNeighbor]);
				ExcavationSegments[lastNeighbor]->Neighbors.Add(NewSegment);
			}
			if (y != 0)
			{
				int rightCornerNeighbor = i - AreaResolution + 1;
				int topCornerNeighbor = i - AreaResolution;
				int leftCornerNeighbor = i - AreaResolution - 1;
				
				NewSegment->Neighbors.Add(ExcavationSegments[topCornerNeighbor]);
				ExcavationSegments[topCornerNeighbor]->Neighbors.Add(NewSegment);

				if (x != 0)
				{
					NewSegment->Neighbors.Add(ExcavationSegments[leftCornerNeighbor]);
					ExcavationSegments[leftCornerNeighbor]->Neighbors.Add(NewSegment);
				}

				if (x != AreaResolution)
				{
					NewSegment->Neighbors.Add(ExcavationSegments[rightCornerNeighbor]);
					ExcavationSegments[rightCornerNeighbor]->Neighbors.Add(NewSegment);
				}

			}
			i++;
		}
	}
	bIsArenaGenerated = true;
}

void AExcavationArea::RefreshArena()
{
	for (UExcavationSegment* Segment : ExcavationSegments)
	{
		Segment->RefreshMesh();
	}
}

void AExcavationArea::DestroyArea()
{
	ExcavationSegments.Empty();
}
