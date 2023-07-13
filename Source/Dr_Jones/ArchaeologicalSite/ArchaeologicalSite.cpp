// Property of Night Shift Games, all rights reserved.

#include "ArchaeologicalSite.h"

#include "DynamicMesh/MeshNormals.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities.h"

AArchaeologicalSite::AArchaeologicalSite() 
{
	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMeshComponent"));
}

void AArchaeologicalSite::TestDig(FVector Direction, FTransform SphereOrigin)
{
	const APlayerController* Controller = UGameplayStatics::GetPlayerController(GWorld, 0);
	SphereOrigin.SetToRelativeTransform(GetTransform());
	
	int32 ViewportX, ViewportY;
	Controller->GetViewportSize(ViewportX, ViewportY);
	const FVector2D ScreenCenter = FVector2D(ViewportX / 2, ViewportY / 2);

	FVector WorldLocation, WorldDirection;
	if (!Controller->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
	{
		return;
	}
	WorldDirection.Normalize();
	
	const float Radius = 100.f;
	const FVector DigPoint = WorldDirection * -Radius + SphereOrigin.GetLocation();

	DynamicMeshComponent->GetDynamicMesh()->EditMesh([&](FDynamicMesh3& EditMesh)
	{
		for (const int32 VertexID : EditMesh.VertexIndicesItr())
		{
			const FVector3d Pos = EditMesh.GetVertex(VertexID);
			if (Utilities::IsPointInSphere(Pos, SphereOrigin.GetLocation(), Radius))
			{
				const FVector NewPos = CalculateSphereDeform(Pos, SphereOrigin.GetLocation(), Radius, DigPoint);
				EditMesh.SetVertex(VertexID, NewPos, false);
				DynamicMeshComponent->NotifyMeshUpdated();
			}
		}
	});
}

FVector AArchaeologicalSite::CalculateSphereDeform(const FVector& VertexPosition, const FVector& SphereOrigin, const float SphereRadius, const FVector& DigDirection) const
{
	// Calculate Direction Vector between Vertex and selected point on a sphere
	FVector VertexDirection = VertexPosition - DigDirection;
	const double HalfSize = VertexDirection.Length();
	VertexDirection.Normalize();

	// Calculate Direction Vector between Origin and selected point on a sphere
	FVector OriginDirection = SphereOrigin - DigDirection;
	OriginDirection.Normalize();

	const float ChordLenght = Utilities::GetChordLength(SphereRadius, VertexDirection, OriginDirection);
	const float DeformLenght = ChordLenght - HalfSize;

	return VertexPosition + VertexDirection * DeformLenght;
}

UDynamicMesh* AArchaeologicalSite::AllocateDynamicMesh()
{
	return NewObject<UDynamicMesh>(this);
}
