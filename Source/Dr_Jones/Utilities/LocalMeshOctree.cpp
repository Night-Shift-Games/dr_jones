// Property of Night Shift Games, all rights reserved.

#include "LocalMeshOctree.h"

#include "Dr_Jones.h"

void BuildLocalMeshVertexOctree(UStaticMesh& Mesh, FLocalMeshVertexOctree& OutOctree)
{
	SCOPED_NAMED_EVENT(BuildLocalMeshVertexOctree, FColorList::DarkTurquoise)

	OutOctree.Destroy();

	if (!ensureMsgf(Mesh.GetNumLODs() > 0, TEXT("Mesh %s has no LODs."), *Mesh.GetName()))
	{
		return;
	}

	FStaticMeshRenderData* MeshRenderData = Mesh.GetRenderData();
	if (!ensureMsgf(MeshRenderData, TEXT("Mesh %s has no render data."), *Mesh.GetName()))
	{
		return;
	}

	const FStaticMeshVertexBuffers& MeshVertexBuffers = MeshRenderData->LODResources[0].VertexBuffers;
	const uint32 NumVertices = MeshVertexBuffers.PositionVertexBuffer.GetNumVertices();
	if (NumVertices == 0)
	{
		UE_LOG(LogDrJones, Warning, TEXT("BuildLocalMeshVertexOctree: Mesh %s has 0 vertices."), *Mesh.GetName());
	}
	for (uint32 I = 0; I < NumVertices; ++I)
	{
		FLocalMeshVertex LocalMeshVertex;
		LocalMeshVertex.Position = MeshVertexBuffers.PositionVertexBuffer.VertexPosition(I);
		LocalMeshVertex.Normal = MeshVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(I);
		LocalMeshVertex.Index = I;

		LocalMeshVertex.BoxCenterAndExtent = FBoxCenterAndExtent(FVector(LocalMeshVertex.Position), FVector(0));
		OutOctree.AddElement(LocalMeshVertex);
	}
}

void ULocalMeshOctree::BuildFromMesh(UStaticMesh* Mesh)
{
	if (!Mesh)
	{
		return;
	}

	BuildLocalMeshVertexOctree(*Mesh, MeshVertexOctree);
}

void ULocalMeshOctree::FindVerticesInBoundingBox(const FVector& Center, const FVector& Extent, TArray<FLocalMeshOctreeVertex>& Vertices)
{
	MeshVertexOctree.FindElementsWithBoundsTest(FBoxCenterAndExtent(Center, Extent), [&](const FLocalMeshVertex& Vertex)
	{
		FLocalMeshOctreeVertex& OutVertex = Vertices.Emplace_GetRef();
		OutVertex.Position = FVector(Vertex.Position);
		OutVertex.Normal = FVector(Vertex.Normal);
	});
}
