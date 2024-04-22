// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"

#include "NightShiftSettings.generated.h"

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Night Shift Settings"))
class DR_JONES_API UNightShiftSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Data Driven Development")
	TSoftObjectPtr<UDataTable> ArtifactDataTable;
};
