// Property of Night Shift Games, all rights reserved.

#include "ReputationComponent.h"

#include "Dr_Jones.h"

UReputationComponent::UReputationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UReputationComponent::AdjustReputation(const FReputationAdjustmentParams& AdjustmentParams)
{
	ArchaeologistReputation += AdjustmentParams.ScienceWorld;
	TreasureHunterReputation += AdjustmentParams.TreasureHunter;

	FReputationChangeData ChangeData;
	ChangeData.ScienceWorldReputationPoints = ArchaeologistReputation;
	ChangeData.TreasureHunterReputationPoints = TreasureHunterReputation;

	if (ReputationPointsToLevelCurve && ReputationPointsToLevelCurve->IsValidLowLevel())
	{
		ChangeData.ScienceWorldLevel = ReputationPointsToLevelCurve->GetFloatValue(ArchaeologistReputation);

		ChangeData.TreasureHunterLevel = ReputationPointsToLevelCurve->GetFloatValue(TreasureHunterReputation);
	}
	else
	{
		UE_LOG(LogDrJones, Error, TEXT("ReputationPointsToLevelCurve has not been set."));
	}

	OnReputationChange.Broadcast(ChangeData);

	ReputationUtils::LogDebugIfEnabled(TEXT("Reputation has been adjusted."));
	ReputationUtils::LogDebugIfEnabled(TEXT("SW: %i -> %i"), ArchaeologistReputation - AdjustmentParams.ScienceWorld, ArchaeologistReputation);
	ReputationUtils::LogDebugIfEnabled(TEXT("TH: %i -> %i"), TreasureHunterReputation - AdjustmentParams.TreasureHunter, TreasureHunterReputation);
}

int32 UReputationComponent::GetReputation(EReputationType ReputationType) const
{
	if (!ReputationPointsToLevelCurve || !ReputationPointsToLevelCurve->IsValidLowLevel())
	{
		UE_LOG(LogDrJones, Error, TEXT("ReputationPointsToLevelCurve has not been set."));
		return 0;
	}
	
	return ReputationType == EReputationType::Archaeologist ? ArchaeologistReputation : TreasureHunterReputation;
}
