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

float UReputationComponent::GetTotalReputationNeededForLevel(int Level) const
{
	return LevelCurveFloat ? LevelCurveFloat->GetFloatValue(static_cast<float>(Level)) : static_cast<float>(Level) * 100.f;
}

void UReputationComponent::LevelUp()
{
	CurrentLevel += 1;
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
	Morality = static_cast<float>(ArchaeologistReputation) / static_cast<float>(GetReputationCombined());

	if (GetTotalReputationNeededForLevel(CurrentLevel + 1) <= GetReputationCombined())
	{
		LevelUp();
	}
	
	OnReputationUpdated.Broadcast();
}
