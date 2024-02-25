// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ReputationComponent.generated.h"

class UDrJonesWidgetBase;

inline TAutoConsoleVariable CVarReputationDebug(
	TEXT("NS.Reputation.Debug"),
	false,
	TEXT("Enable Reputation system debug logging."),
	ECVF_Cheat
);

UENUM(BlueprintType)
enum class EReputationType : uint8
{
	Archaeologist = 0,
	TreasureHunter,
};

UCLASS(ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent),
	HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class DR_JONES_API UReputationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UReputationComponent();

	UFUNCTION(BlueprintCallable, Category = "Night Shift|Reputation")
	void AddReputation(EReputationType Faction, int32 ReputationToAdd);
	
	UFUNCTION(BlueprintPure, Category = "Reputation")
	int32 GetReputation(EReputationType ReputationType) const;

	UFUNCTION(BlueprintPure, Category = "Reputation")
	float GetMorality() const { return Morality; }
private:

	void SetReputation(EReputationType Faction, int32 NewReputation);
	
private:
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Reputation", meta = (AllowPrivateAccess = true))
	float Morality = 0.5;
	
	// "Positive" reputation
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Reputation", meta = (AllowPrivateAccess = true))
	int32 ArchaeologistReputation = 0;

	// "Negative" reputation
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Reputation", meta = (AllowPrivateAccess = true))
	int32 TreasureHunterReputation = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reputation", meta = (AllowPrivateAccess = true))
	TSubclassOf<UDrJonesWidgetBase> ReputationBarWidgetClass;
};