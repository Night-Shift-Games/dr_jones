// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/ArtifactAnimationDataAsset.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "Events/Event.h"

#include "NightShiftSettings.generated.h"

class UDefaultSoundBank;

USTRUCT(BlueprintType)
struct FAudioSettingsData
{
	GENERATED_BODY()

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USoundSubmix> MasterSubmix;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USoundSubmix> SFXSubmix;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USoundSubmix> MusicSubmix;
};

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Night Shift Settings"))
class DR_JONES_API UNightShiftSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Data Driven Development")
	TSoftObjectPtr<UDataTable> ArtifactDataTable;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<UDefaultSoundBank> DefaultSoundBank;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Audio")
	FAudioSettingsData AudioSettings;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Clock")
	FDateTime InitialTime = FDateTime(1922, 11, 4, 15, 30);

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Clock")
	FTimespan TimeToApplyPerSecond = FTimespan(0,1,0);

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Events")
	TArray<TSoftClassPtr<UEvent>> Events;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Events")
	TArray<TSoftClassPtr<UEvent>> StaticEvents;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSoftObjectPtr<UArtifactAnimationDataAsset> FallbackArtifactAnimDataAsset;

	static UDefaultSoundBank* LoadDefaultSoundBank();
};
