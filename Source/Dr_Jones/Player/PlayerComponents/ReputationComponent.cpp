// Property of Night Shift Games, all rights reserved.

#include "ReputationComponent.h"

#include "Dr_Jones.h"

UReputationComponent::UReputationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UReputationComponent::AdjustReputation(const FReputationAdjustmentParams& AdjustmentParams)
{
	ScienceWorldReputation += AdjustmentParams.ScienceWorld;
	TreasureHunterReputation += AdjustmentParams.TreasureHunter;

	FReputationChangeData ChangeData;
	ChangeData.ScienceWorldReputationPoints = ScienceWorldReputation;
	ChangeData.TreasureHunterReputationPoints = TreasureHunterReputation;

	if (ReputationPointsToLevelCurve && ReputationPointsToLevelCurve->IsValidLowLevel())
	{
		ChangeData.ScienceWorldLevel = ReputationPointsToLevelCurve->GetFloatValue(ScienceWorldReputation);
		ChangeData.bScienceWorldLevelChanged = ChangeData.ScienceWorldLevel != CachedScienceWorldLevel;
		CachedScienceWorldLevel = ChangeData.ScienceWorldLevel;

		ChangeData.TreasureHunterLevel = ReputationPointsToLevelCurve->GetFloatValue(TreasureHunterReputation);
		ChangeData.bTreasureHunterLevelChanged = ChangeData.TreasureHunterLevel != CachedTreasureHunterLevel;
		CachedTreasureHunterLevel = ChangeData.TreasureHunterLevel;
	}
	else
	{
		UE_LOG(LogDrJones, Error, TEXT("ReputationPointsToLevelCurve has not been set."));
	}

	OnReputationChange.Broadcast(ChangeData);

	ReputationUtils::LogDebugIfEnabled(TEXT("Reputation has been adjusted."));
	ReputationUtils::LogDebugIfEnabled(TEXT("SW: %i -> %i"), ScienceWorldReputation - AdjustmentParams.ScienceWorld, ScienceWorldReputation);
	ReputationUtils::LogDebugIfEnabled(TEXT("TH: %i -> %i"), TreasureHunterReputation - AdjustmentParams.TreasureHunter, TreasureHunterReputation);
}

int32 UReputationComponent::GetReputationLevel(EReputationType ReputationType) const
{
	if (!ReputationPointsToLevelCurve || !ReputationPointsToLevelCurve->IsValidLowLevel())
	{
		UE_LOG(LogDrJones, Error, TEXT("ReputationPointsToLevelCurve has not been set."));
		return 0;
	}

	int32 ReputationPoints;
	switch (ReputationType)
	{
	case EReputationType::ScienceWorld:
		ReputationPoints = ScienceWorldReputation;
		break;
	case EReputationType::TreasureHunter:
		ReputationPoints = TreasureHunterReputation;
		break;
	default:
		checkNoEntry();
	}

	return ReputationPointsToLevelCurve->GetFloatValue(ReputationPoints);
}

void UReputationComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedScienceWorldLevel = GetReputationLevel(EReputationType::ScienceWorld);
	CachedTreasureHunterLevel = GetReputationLevel(EReputationType::TreasureHunter);
}
