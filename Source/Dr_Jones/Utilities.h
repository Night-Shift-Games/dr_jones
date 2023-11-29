#pragma once

class ADrJonesCharacter;

namespace Utilities
{
	ADrJonesCharacter& GetPlayerCharacter(const UObject& WorldContextObject);
	float CalculateAngleBetweenTwoVectors(const FVector& Direction, const FVector& Second);
	float GetChordLength(const float SphereRadius, const FVector& Direction, const FVector& Second);
	bool IsPointInSphere(const FVector& Point, const FVector& SphereOrigin, const float Radius);
	FVector FindGround(const UObject& WorldContextObject, const FVector& StartLocation, const TArray<AActor*>& ActorsToIgnore);
	uint32 WrapIndexToSize(int64 Index, uint32 Size);
	
	template <typename T>
	uint32 WrapIndexToArray(int64 Index, TArray<T> Array)
	{
		return WrapIndexToSize(Index, Array.Num());
	}

	template <typename T>
	uint32 WrapIndexToArray(int64 Index, TSet<T> Array)
	{
		return WrapIndexToSize(Index, Array.Num());
	}
}	
