// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dr_Jones.h"

#include "ReputationComponent.generated.h"

static TAutoConsoleVariable CVarReputationDebug(
	TEXT("NS.Reputation.Debug"),
	false,
	TEXT("Enable Reputation system debug logging."),
	ECVF_Cheat
);

USTRUCT(BlueprintType, Category = "Reputation")
struct FReputationAdjustmentParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ScienceWorld;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TreasureHunter;
};

USTRUCT(BlueprintType, Category = "Reputation")
struct FReputationChangeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ScienceWorldReputationPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ScienceWorldLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bScienceWorldLevelChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TreasureHunterReputationPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TreasureHunterLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTreasureHunterLevelChanged;
};

UENUM(BlueprintType)
enum class EReputationType : uint8
{
	ScienceWorld,
	TreasureHunter,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReputationChange, const FReputationChangeData&, ShiftData);

UCLASS(ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent),
	HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class DR_JONES_API UReputationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UReputationComponent();

	int32 GetScienceWorldReputation() const { return ScienceWorldReputation; }
	int32 GetTreasureHunterReputation() const { return TreasureHunterReputation; }

	UFUNCTION(BlueprintCallable, Category = "Reputation")
	void AdjustReputation(const FReputationAdjustmentParams& AdjustmentParams);

	UFUNCTION(BlueprintPure, Category = "Reputation")
	int32 GetReputationLevel(EReputationType ReputationType) const;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Reputation")
	FOnReputationChange OnReputationChange;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reputation")
	TObjectPtr<UCurveFloat> ReputationPointsToLevelCurve;

private:
	// "Positive" reputation
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Reputation", meta = (AllowPrivateAccess = true))
	int32 ScienceWorldReputation = 0;

	// "Negative" reputation
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Reputation", meta = (AllowPrivateAccess = true))
	int32 TreasureHunterReputation = 0;

	int32 CachedScienceWorldLevel = 0;
	int32 CachedTreasureHunterLevel = 0;
};

// TODO: Make a generic logger
namespace ReputationUtils
{
#if !NO_LOGGING
	static bool IsDebugEnabled() { return CVarReputationDebug.GetValueOnGameThread(); }

	template<int32 Length, typename... Types>
	static void LogDebug(const TCHAR(&Format)[Length], Types&&... Args)
	{
		check(GEngine);
		const FString Message = FString::Printf(Format, Forward<Types>(Args)...);
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Turquoise, Message);
		// Yep. TEXT("%s"). I don't care...
		UE_LOG(LogDrJones, Log, TEXT("%s"), *Message);
	}

	template<int32 Length, typename... Types>
	static void LogDebugIfEnabled(const TCHAR(&Format)[Length], Types&&... Args)
	{
		if (IsDebugEnabled())
		{
			LogDebug(Format, Forward<Types>(Args)...);
		}
	}
#else
	static constexpr bool IsQuestSystemDebugEnabled() { return false; }
	template<int32 Length, typename... Types> static void LogDebug(const FString& Message) { }
	template<int32 Length, typename... Types> static void LogDebugIfEnabled(const FString& Message) { }
#endif
}

