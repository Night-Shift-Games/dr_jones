// Property of Night Shift Games, all rights reserved.

#include "RestPlaceComponent.h"

#include "Interaction/InteractableComponent.h"

URestPlaceComponent::URestPlaceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URestPlaceComponent::Rest(const FTimespan TimeOffset, ADrJonesCharacter* Player)
{
	if (!Player || FullSequenceTimerHandle.IsValid())
	{
		return;
	}

	if (TimeOffset.IsZero())
	{
		UE_LOG(LogDrJones, Error, TEXT("Cannot rest for 0 or less seconds."));
		return;
	}

	const UWorld* World = GetWorld();
	AIlluminati* Illuminati = AIlluminati::GetIlluminatiInstance(World);
	if (!Illuminati)
	{
		UE_LOG(LogDrJones, Error, TEXT("Resting requires the Illuminati object, but none was found."));
		return;
	}

	APlayerController* PlayerController = Player->GetController<APlayerController>();
	if (!PlayerController)
	{
		UE_LOG(LogDrJones, Error, TEXT("Resting requires the Player to have a controller, but none was found."));
		return;
	}

	Player->DisableInput(PlayerController);

	FTimerManager& TimerManager = World->GetTimerManager();

	// The length of the whole sequence
	TimerManager.SetTimer(FullSequenceTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]
	{
		FullSequenceTimerHandle.Invalidate();
		RestTimerHandle.Invalidate();
		FadeInTimerHandle.Invalidate();
	}), RestFadeTime * 2.0f + 2.0f, false);

	// Fade to black before skipping time
	TimerManager.SetTimer(RestTimerHandle, FTimerDelegate::CreateLambda([Illuminati, TimeOffset]
	{
		check(Illuminati->IsValidLowLevel());
		Illuminati->Clock->SkipTime(TimeOffset);
	}), RestFadeTime, false);

	check(PlayerController->PlayerCameraManager);

	PlayerController->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, RestFadeTime, FLinearColor::Black, false, true);
	
	TimerManager.SetTimer(FadeInTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this, Player, PlayerController]
	{
		if (Player->IsValidLowLevel() && PlayerController->IsValidLowLevel())
		{
			PlayerController->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, RestFadeTime, FLinearColor::Black, false, false);
			Player->EnableInput(PlayerController);
		}
	}), RestFadeTime + 2.0f, false);
}

void URestPlaceComponent::OnInteract(ADrJonesCharacter* Player)
{
	Rest(DefaultRestingTime, Player);
}

void URestPlaceComponent::BeginPlay()
{
	Super::BeginPlay();

	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UInteractableComponent* InteractableComponent = Owner->FindComponentByClass<UInteractableComponent>();
	if (!InteractableComponent)
	{
		return;
	}

	InteractableComponent->InteractDelegate.AddDynamic(this, &URestPlaceComponent::OnInteract);
}
