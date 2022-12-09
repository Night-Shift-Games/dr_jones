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

void AExcavationArea::BeginPlay()
{
	Super::BeginPlay();
	CreateArea();
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
