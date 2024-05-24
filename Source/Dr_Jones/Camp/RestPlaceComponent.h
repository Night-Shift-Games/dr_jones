// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "World/Illuminati.h"

#include "RestPlaceComponent.generated.h"

class ADrJonesCharacter;
struct FWorldClockTime;

UCLASS(ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent), HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class DR_JONES_API URestPlaceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URestPlaceComponent();

	UFUNCTION(BlueprintCallable, Category = "Rest", meta = (Keywords = "Time"))
	void Rest(const FTimespan TimeOffset, ADrJonesCharacter* Player);

	UFUNCTION()
	void OnInteract(ADrJonesCharacter* Player);

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rest")
	FTimespan DefaultRestingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rest")
	float RestFadeTime = 1.0f;

private:
	FTimerHandle RestTimerHandle;
	FTimerHandle FadeInTimerHandle;
	FTimerHandle FullSequenceTimerHandle;
};
