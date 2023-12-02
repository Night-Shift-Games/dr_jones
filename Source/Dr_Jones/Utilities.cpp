#include "Utilities.h"

#include "Kismet/GameplayStatics.h"
#include "Player/DrJonesCharacter.h"
#include "Player/WidgetManager.h"

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

	UWidgetManager& GetWidgetManager(const UObject& WorldContextObject)
	{
		const ADrJonesCharacter& PlayerCharacter = GetPlayerCharacter(WorldContextObject);
		return *PlayerCharacter.GetWidgetManager();
	}

	UDrJonesWidgetBase* GetWidget(const UObject& WorldContextObject, const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
	{
		return GetWidgetManager(WorldContextObject).GetWidget(WidgetClass);
	}
}
