// Property of Night Shift Games, all rights reserved.


#include "ExcavationSegment.h"

UExcavationSegment::UExcavationSegment(const FObjectInitializer& ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer)
{
	bFullResolution = false;
}

void UExcavationSegment::GenerateMesh(int Resolution, float Size)
{
	
	if (!bFullResolution)
	{
		FullResolution = Resolution;
		FullSize = Size;
		Resolution = 4;
	}

	ClearAllMeshSections();

	vertices.Empty();
	Triangles.Empty();
	normals.Empty();
	UV0.Empty();
	tangents.Empty();
	vertexColors.Empty();

	int it = 0;

	float SizeRes = (float)Size / ((float)Resolution - 1);
	for (size_t x = 0; x < Resolution; x++)
	{
		for (size_t y = 0; y < Resolution; y++)
		{
			vertices.Add(FVector(y * SizeRes, x * SizeRes, FMath::RandRange(-1, 1)) + FVector(-Size / 2, -Size / 2, 0));
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
			if (y == Resolution - 1 || x == Resolution - 1 || x == 0 || y == 0) vertices.Last().Z = 0;
			it++;
		}
	}
	CreateMeshSection(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);

	SetMaterial(0, Material);
}

void UExcavationSegment::Dig(FTransform CollisionPoint, FVector Dig)
{
	if (!bFullResolution)
	{
		bFullResolution = true;
		GenerateMesh(FullResolution, FullSize);
	}
	for (size_t i = 0; i < vertices.Num(); i++)
	{
		if (UKismetMathLibrary::IsPointInBoxWithTransform(vertices[i], CollisionPoint, FVector(30, 30, 30))) vertices[i] += Dig;
	}
	RefreshMesh();
}

void UExcavationSegment::RefreshMesh()
{
	ClearAllMeshSections();
	CreateMeshSection(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
}