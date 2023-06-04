// Property of Night Shift Games, all rights reserved.

#include "ArchaeologicalSite.h"

#include "DynamicMesh/MeshNormals.h"
#include "Kismet/KismetMathLibrary.h"

AArchaeologicalSite::AArchaeologicalSite() 
{
	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMeshComponent"));
}

void AArchaeologicalSite::TestDig(FTransform SphereOrigin)
{
	SphereOrigin.SetToRelativeTransform(GetTransform());
	DynamicMeshComponent->GetDynamicMesh()->EditMesh([&](FDynamicMesh3& EditMesh)
	{
		for (const int32 VertexID : EditMesh.VertexIndicesItr())
		{
			FVector3d Pos = EditMesh.GetVertex(VertexID);
			FVector3d Normal = UE::Geometry::FMeshNormals::ComputeVertexNormal(EditMesh, VertexID);
			if (UKismetMathLibrary::IsPointInBox(Pos, SphereOrigin.GetLocation(), FVector(50, 50, 50)))
			{
				EditMesh.SetVertex(VertexID, Pos += FVector(0,0,-25), false);
				DynamicMeshComponent->NotifyMeshUpdated();
			}
		}
	});
}

UDynamicMesh* AArchaeologicalSite::AllocateDynamicMesh()
{
	return NewObject<UDynamicMesh>(this);
}
