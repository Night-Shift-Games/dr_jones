// Property of Night Shift Games, all rights reserved.

#include "ExcavationSegment.h"
#include "Kismet/KismetMathLibrary.h"

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
			vertices.Add(FVector(y * SizeRes, x * SizeRes, 0) + FVector(-Size / 2, -Size / 2, 0));
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
		if (UKismetMathLibrary::IsPointInBoxWithTransform(vertices[i], CollisionPoint, FVector(30, 30, 30)))
		{

			bool bIsBottomEdge = i > vertices.Num() - FullResolution - 2;
			bool bIsTopEdge = i < FullResolution;
			bool bIsRightEdge = (i % FullResolution == 0);
			bool bIsLeftEdge = (i == 0 || (i+1) % FullResolution == 0);
			bool bIsEdge = bIsBottomEdge || bIsTopEdge || bIsLeftEdge || bIsRightEdge;

			if (bSmoothDig && !bIsEdge)
			{
				vertices[i] += Dig + FVector(Dig.X, Dig.Y, Dig.Z * 5);
				vertices[i] =	((
								vertices[i]						+ vertices[i + 1]				+ vertices[i - 1] + 
								vertices[i + FullResolution + 1]+ vertices[i + FullResolution]	+ vertices[i + FullResolution - 1 ] + 
								vertices[i - FullResolution + 1]+ vertices[i - FullResolution]	+ vertices[i - FullResolution - 1]) / 9);
			}
			else
			{
				vertices[i] += Dig;
			}
		}
	}
	RefreshMesh();
}

void UExcavationSegment::RefreshMesh()
{
	ClearAllMeshSections();
	CreateMeshSection(0, vertices, Triangles, normals, UV0, vertexColors, tangents, true);
}
