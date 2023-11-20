// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dr_Jones.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WorldData.h"
#include "Managment/Dr_JonesGameModeBase.h"

#include "WorldDataUtilities.generated.h"

USTRUCT(BlueprintType)
struct FCultureTimeRange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Begin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 End;
};

USTRUCT(BlueprintType)
struct FSingleCultureData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCultureTimeRange> CultureTimeRanges;
};

USTRUCT(BlueprintType)
struct FCombinedCultureData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSingleCultureData> Cultures;
};

UCLASS()
class DR_JONES_API UWorldDataUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category = "World Data", meta = (WorldContext = "WorldContextObject"))
	static FORCEINLINE UWorldData* GetGlobalWorldData(const UObject* WorldContextObject)
	{
		check(GEngine);
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (!World)
		{
			return nullptr;
		}

		const ADr_JonesGameModeBase* GameMode = World->GetAuthGameMode<ADr_JonesGameModeBase>();
		if (!GameMode)
		{
			return nullptr;
		}

		return GameMode->GetGlobalWorldData();
	}

	/** Converts the latitude and longitude to X and Y coordinates on an equirectangularly projected globe */
	UFUNCTION(BlueprintPure, Category = "GeoLocation|Conversion", meta = (CompactNodeTitle = "GeoToEqu"))
	static FORCEINLINE FVector2D ConvertGeoToEquirectangular(FGeoLocation GeoLocation)
	{
		return GeoLocation.ConvertToEquirectangular();
	}

	/** Converts the X and Y coordinates on an equirectangularly projected globe to latitude and longitude */
	UFUNCTION(BlueprintPure, Category = "GeoLocation|Conversion", meta = (CompactNodeTitle = "EquToGeo"))
	static FORCEINLINE FGeoLocation ConvertEquirectangularToGeo(FVector2D Coords)
	{
		return FGeoLocation::MakeFromEquirectangular(Coords);
	}

	UFUNCTION(BlueprintPure, Category = "GeoLocation", meta = (CompactNodeTitle = "Equator"))
	static FORCEINLINE float GetEquatorLength()
	{
		return FGeoLocation::EquatorLength;
	}
	
	UFUNCTION(BlueprintPure, Category = "GeoLocation", meta = (CompactNodeTitle = "Meridian"))
	static FORCEINLINE float GetMeridianLength()
	{
		return FGeoLocation::MeridianLength;
	}

	// USpatialData utilities

	UFUNCTION(BlueprintCallable, Category = "Spatial")
	static FCombinedCultureData SampleCombinedCultureData(UWorldSpatialData* Target, FGeoLocation GeoLocation);

	UFUNCTION(BlueprintCallable, Category = "Spatial")
	static float SampleCulture(UWorldSpatialData* Target, FName CultureID, FGeoLocation GeoLocation, float Year)
	{
		if (!Target)
		{
			UE_LOG(LogDrJones, Error, TEXT("Target was null."));
			return 0.0f;
		}

		return Target->SampleByteNormalized(CultureID, GeoLocation, Year);
	}

	UFUNCTION(BlueprintCallable, Category = "Spatial")
	static float SampleFloatAtLocation(UWorldSpatialData* Target, FName Attribute, FGeoLocation GeoLocation, float Year)
	{
		if (!Target)
		{
			UE_LOG(LogDrJones, Error, TEXT("Target was null."));
			return 0.0f;
		}

		return Target->SampleAtLocation<float>(Attribute, GeoLocation);
	}

	UFUNCTION(BlueprintCallable, Category = "Spatial")
	static int32 SampleIntAtLocation(UWorldSpatialData* Target, FName Attribute, FGeoLocation GeoLocation, float Year)
	{
		if (!Target)
		{
			UE_LOG(LogDrJones, Error, TEXT("Target was null."));
			return 0;
		}
		
		return Target->SampleAtLocation<int32>(Attribute, GeoLocation);
	}

	UFUNCTION(BlueprintCallable, Category = "Spatial")
	static uint8 SampleByteAtLocation(UWorldSpatialData* Target, FName Attribute, FGeoLocation GeoLocation, float Year)
	{
		if (!Target)
		{
			UE_LOG(LogDrJones, Error, TEXT("Target was null."));
			return 0;
		}
		
		return Target->SampleAtLocation<uint8>(Attribute, GeoLocation);
	}
};
