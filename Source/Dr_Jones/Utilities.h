﻿#pragma once
#include "Player/DrJonesCharacter.h"
#include "UI/DrJonesWidgetBase.h"

class ADrJonesCharacter;

#include "Utilities.generated.h"

namespace Utilities
{
	ADrJonesCharacter& GetPlayerCharacter(const UObject& WorldContextObject);
	float CalculateAngleBetweenTwoVectors(const FVector& Direction, const FVector& Second);
	float GetChordLength(const float SphereRadius, const FVector& Direction, const FVector& Second);
	bool IsPointInSphere(const FVector& Point, const FVector& SphereOrigin, const float Radius);
	FVector FindGround(const UObject& WorldContextObject, const FVector& StartLocation, const TArray<AActor*>& ActorsToIgnore);
	uint32 WrapIndexToSize(int64 Index, uint32 Size);
	double GetMeshZOffset(const AActor& Actor);
	UDrJonesWidgetBase* GetWidget(const UObject& WorldContextObject, const TSubclassOf<UDrJonesWidgetBase> WidgetClass);
	UWidgetManager& GetWidgetManager(const UObject& WorldContextObject);
	
	template <class TWidgetClass>
	TWidgetClass* GetWidget(const UObject& WorldContextObject, const TSubclassOf<UDrJonesWidgetBase> WidgetClass)
	{
		UDrJonesWidgetBase* Widget = GetWidget(WorldContextObject, WidgetClass);
		return Widget ? Cast<TWidgetClass>(Widget) : nullptr;
	}

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

UCLASS()
class UDrJonesUtilitiesFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "DrJones|Utilities", meta = (WorldContext = "WorldContextObject"))
	static ADrJonesCharacter* GetDrJonesCharacter(const UObject* WorldContextObject)
	{ return WorldContextObject ? &Utilities::GetPlayerCharacter(*WorldContextObject) : nullptr; }

	UFUNCTION(BlueprintPure, Category = "DrJones|Utilities", meta = (WorldContext = "WorldContextObject"))
	static FVector FindGround(const UObject* WorldContextObject, const FVector& StartLocation, const TArray<AActor*>& ActorsToIgnore)
	{ return WorldContextObject ? Utilities::FindGround(*WorldContextObject, StartLocation, ActorsToIgnore) : FVector::ZeroVector; }

	UFUNCTION(BlueprintPure, Category = "DrJones|Utilities", meta = (WorldContext = "WorldContextObject"))
	static int64 WrapIndexToSize(int64 Index, int32 Size)
	{ return Utilities::WrapIndexToSize(Index, Size); }

	UFUNCTION(BlueprintPure, Category = "DrJones|Utilities", meta = (WorldContext = "WorldContextObject"))
	static FHitResult GetHitResultOfPlayerSight(float Reach = 400.f)
	{ return ADrJonesCharacter::GetPlayerLookingAt(Reach); }
};