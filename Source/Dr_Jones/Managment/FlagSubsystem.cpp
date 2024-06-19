#include "FlagSubsystem.h"

void UFlagSubsystem::SetFlag(FName FlagID)
{
	if (IsFlagSet(FlagID))
	{
		return;
	}
	GlobalFlags.Add(FlagID);
	OnFlagAdded.Broadcast(FlagID);
}

bool UFlagSubsystem::IsFlagSet(FName FlagID) const
{
	return GlobalFlags.Contains(FlagID);
}
