#include "Utilities.h"

#include "Kismet/GameplayStatics.h"
#include "Player/DrJonesCharacter.h"

ADrJonesCharacter& Utilities::GetPlayerCharacter(const UObject& WorldContextObject)
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(&WorldContextObject, 0);
	return *Cast<ADrJonesCharacter>(PlayerCharacter);
}

float Utilities::CalculateAngleBetweenTwoVectors(const FVector& Direction, const FVector& Second)
{
	return FMath::Acos(FVector::DotProduct(Direction, Second));
}

float Utilities::GetChordLength(const float SphereRadius, const FVector& Direction, const FVector& Second)
{
	const float Angle = CalculateAngleBetweenTwoVectors(Direction, Second);
	const float CenterAngle = PI - (2 * Angle);
	return SphereRadius * 2 * FMath::Sin(CenterAngle / 2);
}

uint32 Utilities::WrapIndexToSize(int64 Index, const uint32 Size)
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

bool Utilities::IsPointInSphere(const FVector& Point, const FVector& SphereOrigin, const float Radius)
{
	return (SphereOrigin - Point).SquaredLength() < (Radius * Radius);
}
