// Property of Night Shift Games, all rights reserved.

#include "Watch.h"

#include "Interaction/InteractableComponent.h"
#include "World/Clock.h"

AWatch::AWatch()
{
	PrimaryActorTick.bCanEverTick = true;

	WatchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WatchMesh"));
	InteractableComponent->SetupAttachment(WatchMesh);
}

void AWatch::BeginPlay()
{
	Super::BeginPlay();

	ClockPrivate = GetGameInstance()->GetSubsystem<UClock>();
	ClockPrivate->OnClockTick.AddUniqueDynamic(this, &AWatch::OnClockTick);
}

void AWatch::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const FTimespan& TimePerSecond = ClockPrivate->TimeToApplyPerSecond;
	CurrentHourNormalized += TimePerSecond.GetHours() * DeltaSeconds / 12.0f;
	CurrentMinuteNormalized += (TimePerSecond.GetMinutes() + TimePerSecond.GetSeconds() / 60.0f) * DeltaSeconds / 60.0f;

	AdjustClockHands(CurrentHourNormalized, CurrentMinuteNormalized);
}

void AWatch::OnClockTick(FDateTime CurrentDate)
{
	CurrentHourNormalized = CurrentDate.GetHour12() / 12.0f + CurrentDate.GetMinute() / (60.0f * 12.0f);
	CurrentMinuteNormalized = CurrentDate.GetMinute() / 60.0f + CurrentDate.GetSecond() / (60.0f * 60.0f);
}
