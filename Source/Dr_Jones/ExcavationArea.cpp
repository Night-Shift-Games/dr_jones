// Property of Night Shift Games, all rights reserved.


#include "ExcavationArea.h"

AExcavationArea::AExcavationArea() 
{ 
	PrimaryActorTick.bCanEverTick = false; 
	if (!PMC) PMC = CreateDefaultSubobject<UProceduralMeshComponent>("DefaultPMC");
	PMC->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	Resolution = 10;
	Size = 100;

}

void AExcavationArea::BeginPlay()
{
	Super::BeginPlay();
	
	RefreshMesh();

}

void AExcavationArea::PostActorCreated()
{
	Super::PostActorCreated();
	RefreshMesh();
}

void AExcavationArea::PostLoad()
{
	Super::PostLoad();
	RefreshMesh();
}

void AExcavationArea::RefreshMesh()
{
	TArray<FVector> vertices;
	TArray<int32> Triangles;
	TArray<FVector> normals;
	TArray<FVector2D> UV0;
	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertexColors;

	int it = 0;
	for (size_t x = 0; x < Resolution; x++)
	{
		for (size_t y = 0; y < Resolution; y++)
		{	
			vertices.Add(FVector(y * (float)Size/((float)Resolution - 1), x * (float)Size/((float)Resolution - 1), 0) + FVector(-Size/2, -Size/2, 0));
			if (y != Resolution - 1 && x != Resolution - 1)
			{
				Triangles.Add(it);
				Triangles.Add(it + Resolution);
				Triangles.Add(it + Resolution + 1);

				Triangles.Add(it);
				Triangles.Add(it + Resolution + 1);
				Triangles.Add(it + 1);
			}
			it++;
		}
	}
	PMC->ClearAllMeshSections();
	PMC->CreateMeshSection(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
	PMC->SetMeshSectionVisible(0, true);
	if (ExcavateMaterial) PMC->SetMaterial(0, ExcavateMaterial);
}

void AExcavationArea::CreateFace()
{


}

void AExcavationArea::PostEditChangeProperty (FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	RefreshMesh();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}