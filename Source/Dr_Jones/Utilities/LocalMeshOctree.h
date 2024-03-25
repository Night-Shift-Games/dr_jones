// Property of Night Shift Games, all rights reserved.

#pragma once

#include "LocalMeshOctree.generated.h"

struct FLocalMeshVertex
{
	FBoxCenterAndExtent BoxCenterAndExtent;
	FVector3f Position;
	FVector3f Normal;
	uint32 Index;
};

// Semantics for the local mesh vertex octree */
struct FLocalMeshVertexOctreeSemantics
{
	enum { MaxElementsPerLeaf = 16 };
	enum { MinInclusiveElementsPerNode = 7 };
	enum { MaxNodeDepth = 12 };

	typedef TInlineAllocator<MaxElementsPerLeaf> ElementAllocator;

	FORCEINLINE static FBoxCenterAndExtent GetBoundingBox(const FLocalMeshVertex& Element)
	{
		return Element.BoxCenterAndExtent;
	}

	FORCEINLINE static bool AreElementsEqual(const FLocalMeshVertex& A, const FLocalMeshVertex& B)
	{
		return A.Index == B.Index;
	}

	FORCEINLINE static void SetElementId(const FLocalMeshVertex& Element, FOctreeElementId2 Id)
	{
	}
};

using FLocalMeshVertexOctree = TOctree2<FLocalMeshVertex, FLocalMeshVertexOctreeSemantics>;

void BuildLocalMeshVertexOctree(UStaticMesh& Mesh, FLocalMeshVertexOctree& OutOctree);

USTRUCT(BlueprintType)
struct FLocalMeshOctreeVertex
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Normal;
};

UCLASS(Blueprintable)
class ULocalMeshOctree : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DrJones|LocalMeshOctree")
	void BuildFromMesh(UStaticMesh* Mesh);

	UFUNCTION(BlueprintCallable, Category = "DrJones|LocalMeshOctree")
	void FindVerticesInBoundingBox(const FVector& Center, const FVector& Extent, TArray<FLocalMeshOctreeVertex>& Vertices);

	FLocalMeshVertexOctree MeshVertexOctree;
};
