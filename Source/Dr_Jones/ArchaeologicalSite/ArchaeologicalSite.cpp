// Property of Night Shift Games, all rights reserved.

#include "ArchaeologicalSite.h"

#include "Components/SphereComponent.h"
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
		float Radius = 50.f;
		for (const int32 VertexID : EditMesh.VertexIndicesItr())
		{
			FVector3d Pos = EditMesh.GetVertex(VertexID);
			FVector3d Normal = UE::Geometry::FMeshNormals::ComputeVertexNormal(EditMesh, VertexID);
			if (IsPointInSphere(Pos, SphereOrigin.GetLocation(), 50))
			{
				FVector Dir = Pos - SphereOrigin.GetLocation();
				const double HalfSize = Dir.Length();
				Dir.Normalize();
				FVector NewPos = Pos + Dir * (Radius - HalfSize);
				DrawDebugLine(GWorld, GetActorLocation() + SphereOrigin.GetLocation(), GetActorLocation() + NewPos, FColor::Red, false, 10.f);
				DrawDebugSphere(GWorld, GetActorLocation() + SphereOrigin.GetLocation(), Radius, 10, FColor::Green, false, 10.f);
				EditMesh.SetVertex(VertexID, NewPos, false);
				DynamicMeshComponent->NotifyMeshUpdated();
			}
		}
	});
}

UDynamicMesh* AArchaeologicalSite::AllocateDynamicMesh()
{
	return NewObject<UDynamicMesh>(this);
}

bool AArchaeologicalSite::IsPointInSphere(const FVector& Point, const FVector& SphereOrigin, float Radius) const
{
	return (SphereOrigin - Point).Length() < Radius;
}
