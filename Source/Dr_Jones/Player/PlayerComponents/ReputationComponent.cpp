// Property of Night Shift Games, all rights reserved.

#include "ReputationComponent.h"

UReputationComponent::UReputationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UReputationComponent::AddReputation(EReputationType Faction, int32 ReputationToAdd)
{
	SetReputation(Faction, GetReputation(Faction) + ReputationToAdd);
}

int32 UReputationComponent::GetReputation(EReputationType ReputationType) const
{
	return ReputationType == EReputationType::Archaeologist ? ArchaeologistReputation : TreasureHunterReputation;
}

void UReputationComponent::SetReputation(EReputationType Faction, int32 NewReputation)
{
	if (Faction == EReputationType::Archaeologist)
	{
		ArchaeologistReputation = NewReputation;
	}
	else
	{
		TreasureHunterReputation = NewReputation;
	}
	Morality = ArchaeologistReputation / (ArchaeologistReputation + TreasureHunterReputation);
}
