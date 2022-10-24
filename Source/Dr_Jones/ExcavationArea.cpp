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
	
	CreateMesh();

}

void AExcavationArea::PostActorCreated()
{
	Super::PostActorCreated();
	CreateMesh();
}

void AExcavationArea::PostLoad()
{
	Super::PostLoad();
	CreateMesh();
}

void AExcavationArea::CreateMesh()
{
	PMC->ClearAllMeshSections();
	
	vertices.Empty();
	Triangles.Empty();;
	normals.Empty();;
	UV0.Empty();;
	tangents.Empty();;
	vertexColors.Empty();;

	int it = 0;

	float SizeRes = (float)Size / ((float)Resolution - 1);
	for (size_t x = 0; x < Resolution; x++)
	{
		for (size_t y = 0; y < Resolution; y++)
		{	
			vertices.Add(FVector(y * SizeRes, x * SizeRes, FMath::RandRange(-2, 2)) + FVector(-Size/2, -Size/2, 0));
			UV0.Add(FVector2D(y * SizeRes / 100, x * SizeRes / 100));
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
	PMC->CreateMeshSection(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
	if (ExcavateMaterial) PMC->SetMaterial(0, ExcavateMaterial);
}

void AExcavationArea::RefreshMesh()
{
	PMC->ClearAllMeshSections();
	PMC->CreateMeshSection(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
}

void AExcavationArea::Dig()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5, FColor::Red, (GetActorLocation() + CollisionBox).ToString());

	for (size_t i = 0; i < vertices.Num(); i++)
	{
		if (UKismetMathLibrary::IsPointInBox(vertices[i], CollisionBox, FVector(30, 30, 30)))
		vertices[i] += FVector(0, 0, -10);
	}

	RefreshMesh();
}

void AExcavationArea::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == "Resolution" || PropertyName == "Size" || PropertyName == "Excavate Material") CreateMesh();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}