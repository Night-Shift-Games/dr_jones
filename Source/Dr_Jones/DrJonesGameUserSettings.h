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
	UFUNCTION(BlueprintCallable, Category = "DrJones|Settings")
	static UDrJonesGameUserSettings* GetDrJonesGameUserSettings();

	UFUNCTION(BlueprintPure, Category = "DrJones|Settings")
	static TArray<FIntPoint> GetSupportedDisplayResolutions();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "DrJones|Settings", meta = (WorldContext = "WorldContextObject"))
	void ApplyAudioSettings(const UObject* WorldContextObject) const;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Input")
	float LookSensitivity = 1.0f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float MasterVolume = 1.0f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float SFXVolume = 1.0f;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float MusicVolume = 1.0f;
};
