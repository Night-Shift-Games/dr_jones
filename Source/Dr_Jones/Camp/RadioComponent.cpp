// Property of Night Shift Games, all rights reserved.

#include "RadioComponent.h"

#include "Kismet/GameplayStatics.h"
#include "World/Illuminati.h"

URadioComponent::URadioComponent()
{
#if WITH_EDITORONLY_DATA
	bVisualizeComponent = true;
#endif
}

void URadioComponent::OnGlobalEventReceived(const FIlluminatiGlobalEvent& GlobalEvent)
{
	UGameplayStatics::SpawnSoundAttached(GlobalEvent.RadioSound,
		this,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepRelativeOffset,
		true,
		RadioSoundVolume,
		1.0f,
		0.0f,
		AttenuationSettings,
		ConcurrencySettings,
		true);
}

void URadioComponent::BeginPlay()
{
	Super::BeginPlay();

	FIlluminatiDelegates::OnGlobalEventReceived.AddUObject(this, &URadioComponent::OnGlobalEventReceived);
}
