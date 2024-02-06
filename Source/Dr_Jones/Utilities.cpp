#include "Utilities.h"

#include "Kismet/GameplayStatics.h"
#include "Player/DrJonesCharacter.h"
#include "Player/WidgetManager.h"
#include "Player/PlayerComponents/InteractionComponent.h"

namespace Utilities
{
	ADrJonesCharacter& GetPlayerCharacter(const UObject& WorldContextObject)
	{
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(&WorldContextObject, 0);
		return *Cast<ADrJonesCharacter>(PlayerCharacter);
	}

	float CalculateAngleBetweenTwoVectors(const FVector& Direction, const FVector& Second)
	{
		return FMath::Acos(FVector::DotProduct(Direction, Second));
	}

	float GetChordLength(const float SphereRadius, const FVector& Direction, const FVector& Second)
	{
		const float Angle = CalculateAngleBetweenTwoVectors(Direction, Second);
		const float CenterAngle = PI - (2 * Angle);
		return SphereRadius * 2 * FMath::Sin(CenterAngle / 2);
	}

	bool IsPointInSphere(const FVector& Point, const FVector& SphereOrigin, const float Radius)
	{
		return (SphereOrigin - Point).SquaredLength() < (Radius * Radius);
	}

	FVector FindGround(const UObject& WorldContextObject, const FVector& StartLocation, const TArray<AActor*>& ActorsToIgnore)
	{
		UWorld* World = WorldContextObject.GetWorld();
		if (!World)
		{
			return FVector::ZeroVector;
		}
		FHitResult Hit;
		const FVector End = StartLocation + FVector(0.f, 0.f, -300.f);
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(ActorsToIgnore);
		World->LineTraceSingleByChannel(Hit, StartLocation, End, ECC_Visibility, QueryParams);
		return Hit.Location;
	}

	uint32 WrapIndexToSize(int64 Index, const uint32 Size)
	{
		if (Size == 0)
		{
			return 0;
		}
		if (Index < 0)
		{
			return Size - 1;
		}
		if (Index >= Size)
		{
			return 0;
		}
		return Index;
	}

	double GetMeshZOffset(const AActor& Actor)
	{
		const UMeshComponent* Mesh = Actor.FindComponentByClass<UMeshComponent>();
		double ZOffset = Mesh->GetLocalBounds().BoxExtent.Z;
		const FVector Origin = Mesh->GetLocalBounds().Origin;
		ZOffset = Origin.Z - ZOffset;
		return ZOffset;
	}
	
	UWidgetManager& GetWidgetManager(const UObject& WorldContextObject)
	{
		const ADrJonesCharacter& PlayerCharacter = GetPlayerCharacter(WorldContextObject);
		return *PlayerCharacter.GetWidgetManager();
	}

	UDrJonesWidgetBase* GetWidget(const UObject& WorldContextObject, const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
	{
		return GetWidgetManager(WorldContextObject).GetWidget(WidgetClass);
	}

	FHitResult GetPlayerLookingAt(const float Reach)
	{
		// TODO: Add WCO
		UWorld* World = GWorld;
		if (!World)
		{
			return {};
		}
		const APlayerController* Controller = UGameplayStatics::GetPlayerController(World, 0);

		int32 ViewportX, ViewportY;
		Controller->GetViewportSize(ViewportX, ViewportY);
		FVector2D ScreenCenter = FVector2D(ViewportX / 2.0, ViewportY / 2.0);

		FVector WorldLocation, WorldDirection;
		if (!Controller->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
		{
			return {};
		}

		FVector LineEnd = WorldLocation + WorldDirection * Reach;
		FHitResult Hit;
	
		World->LineTraceSingleByChannel(Hit, WorldLocation, LineEnd, ECC_Visibility);

		if (!Hit.bBlockingHit)
		{
			Hit.Location = LineEnd;
		}
#if ENABLE_DRAW_DEBUG
		Utilities::DrawInteractionDebugInfo(WorldLocation, LineEnd, Hit);
#endif
	
		return Hit;
	}

#if ENABLE_DRAW_DEBUG
	void DrawInteractionDebugInfo(const FVector& WorldLocation, const FVector& LineEnd, const FHitResult& Hit)
	{
		if (CVarInteraction.GetValueOnAnyThread() == 0)
		{
			return;
		}
		DrawDebugLine(GWorld, WorldLocation, LineEnd, FColor::Red, false, 0, 0, 1);
		FString Debug = FString(TEXT("Pointed Actor: "));
		if (const AActor* Actor = Hit.GetActor())
		{
			Debug += Actor->GetName();
		}
		GEngine->AddOnScreenDebugMessage(420, 10, FColor::Green, Debug);
	}
#endif
}
