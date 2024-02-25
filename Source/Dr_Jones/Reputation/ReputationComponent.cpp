// Property of Night Shift Games, all rights reserved.

#include "ReputationComponent.h"
#include "UI/DrJonesWidgetBase.h"

#include "Utilities/Utilities.h"

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
	Morality = static_cast<float>(ArchaeologistReputation) / static_cast<float>(GetReputationCombined());
	Utilities::GetWidget(*this, ReputationBarWidgetClass)->UpdateData();
}
