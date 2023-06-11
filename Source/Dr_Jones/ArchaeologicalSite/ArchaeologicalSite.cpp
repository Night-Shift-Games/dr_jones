// Property of Night Shift Games, all rights reserved.

#include "ArchaeologicalSite.h"

#include "DynamicMesh/MeshNormals.h"
#include "Kismet/GameplayStatics.h"

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
			FVector3d Pos = EditMesh.GetVertex(VertexID);
			if (IsPointInSphere(Pos, SphereOrigin.GetLocation(), Radius))
			{
				const FVector NewPos = CalculateSphereDeform(Pos, SphereOrigin.GetLocation(), Radius, DigPoint);
				EditMesh.SetVertex(VertexID, NewPos, false);
				DynamicMeshComponent->NotifyMeshUpdated();
			}
		}
	});
}

// TODO: This should end up in some utility
float AArchaeologicalSite::CalculateAngleBetweenTwoVectors(const FVector& Direction, const FVector& Second) const
{
	return FMath::Acos(FVector::DotProduct(Direction, Second));
}

// TODO: This should end up in some utility
float AArchaeologicalSite::GetChordLenght(const float SphereRadius, const FVector& Direction, const FVector& Second) const
{
	const float Angle = CalculateAngleBetweenTwoVectors(Direction, Second);
	const float CenterAngle = PI - (2 * Angle);
	return SphereRadius * 2 * FMath::Sin(CenterAngle / 2);
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

	const float ChordLenght = GetChordLenght(SphereRadius, VertexDirection, OriginDirection);
	const float DeformLenght = ChordLenght - HalfSize;

	return VertexPosition + VertexDirection * DeformLenght;
}

UDynamicMesh* AArchaeologicalSite::AllocateDynamicMesh()
{
	return NewObject<UDynamicMesh>(this);
}

// TODO: This should end up in some utility
bool AArchaeologicalSite::IsPointInSphere(const FVector& Point, const FVector& SphereOrigin, const float Radius) const
{
	return (SphereOrigin - Point).Length() < Radius;
}
