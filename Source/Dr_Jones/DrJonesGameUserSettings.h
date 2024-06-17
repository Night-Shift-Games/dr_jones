// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

#include "DrJonesGameUserSettings.generated.h"

UCLASS()
class DR_JONES_API UDrJonesGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DrJones")
	static UDrJonesGameUserSettings* GetDrJonesGameUserSettings();

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Input")
	float LookSensitivity = 1.0f;
};
