// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "FlagSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalFlagAdded, FName, FlagID);

UCLASS()
class DR_JONES_API UFlagSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetFlag(FName FlagID);

	UFUNCTION(BlueprintPure)
	bool IsFlagSet(FName FlagID) const;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Dr Jones")
	FOnGlobalFlagAdded OnFlagAdded;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSet<FName> GlobalFlags;
};
