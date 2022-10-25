// Property of Night Shift Games, all rights reserved.


#include "ExcavationArea.h"

AExcavationArea::AExcavationArea() 
{ 
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;
	PrimaryActorTick.bCanEverTick = false; 
	Resolution = 10;
	Size = 100;
	AreaResolution = 1;
}

void AExcavationArea::BeginPlay()
{
	Super::BeginPlay();
	
	CreateArea();

}

void AExcavationArea::PostActorCreated()
{
	Super::PostActorCreated();
	CreateArea();
}

void AExcavationArea::PostLoad()
{
	Super::PostLoad();
	CreateArea();
}

void AExcavationArea::CreateArea()
{
	for (auto x : ExcavationSegments)
	{
		x->DestroyComponent();
	}
	
	ExcavationSegments.Empty();

	SegmentSize = Size / AreaResolution;
	
	for (size_t y = 0; y < AreaResolution; y++)
	{
		for (size_t x = 0; x < AreaResolution; x++)
		{
			FTransform SegmentLocation(FVector(SegmentSize * x, SegmentSize * y, 0) + FVector(-Size / 2.f + SegmentSize/2, -Size / 2.f + SegmentSize / 2, 0));
			UExcavationSegment* NewSegment = Cast<UExcavationSegment>(AddComponentByClass(UExcavationSegment::StaticClass(), false, SegmentLocation, false));
			ExcavationSegments.Add(NewSegment);
			NewSegment->GenerateMesh(Resolution, SegmentSize);
		}
	}

}

void AExcavationArea::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	//if (PropertyName == "Resolution" || PropertyName == "Size" || PropertyName == "Excavate Material") CreateMesh();
	//else { ; }
	Super::PostEditChangeProperty(PropertyChangedEvent);
}