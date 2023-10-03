// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "RadioComponent.generated.h"

struct FIlluminatiGlobalEvent;

UCLASS(ClassGroup = DrJones, meta = (BlueprintSpawnableComponent), HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class DR_JONES_API URadioComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	URadioComponent();
	void OnGlobalEventReceived(const FIlluminatiGlobalEvent& GlobalEvent);

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Radio")
	TObjectPtr<USoundAttenuation> AttenuationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Radio")
	TObjectPtr<USoundConcurrency> ConcurrencySettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radio")
	float RadioSoundVolume = 1.0f;
};
