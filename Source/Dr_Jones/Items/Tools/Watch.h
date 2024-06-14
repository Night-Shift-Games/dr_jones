// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"

#include "Watch.generated.h"

class UClock;

UCLASS(Blueprintable)
class DR_JONES_API AWatch : public ATool
{
	GENERATED_BODY()

public:
	AWatch();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnClockTick(FDateTime CurrentDate);

	UFUNCTION(BlueprintImplementableEvent)
	void AdjustClockHands(float HourNormalized, float MinuteNormalized);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> WatchMesh;

private:
	UPROPERTY(Transient)
	TObjectPtr<UClock> ClockPrivate;

	float CurrentHourNormalized = 0.0f;
	float CurrentMinuteNormalized = 0.0f;
};
