// Property of Night Shift Games, all rights reserved.

#include "BlendCameraComponent.h"

#include "Blueprint/UserWidget.h"

UBlendCameraComponent::UBlendCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBlendCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!MainViewTarget)
	{
		MainViewTarget = GetOwner();
	}
}

void UBlendCameraComponent::SetViewTarget(AActor* NewViewTarget, UUserWidget* WidgetToFocus)
{
	if (!NewViewTarget)
	{
		ResetViewTarget();
		return;
	}

	APlayerController* Controller = GetViewingController();
	if (!Controller)
	{
		return;
	}
	
	Controller->SetViewTargetWithBlend(NewViewTarget, 1.0f);

	if (WidgetToFocus)
	{
		Controller->bShowMouseCursor = true;

		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
		Controller->SetInputMode(InputMode);

		const UWorld* World = Controller->GetWorld();
		ensureAlwaysMsgf(World, TEXT("Camera is not in the world!"));
		UGameViewportClient* Viewport = World->GetGameViewport();
		check(Viewport);
		Viewport->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
	}
}

void UBlendCameraComponent::ResetViewTarget()
{
	APlayerController* Controller = GetViewingController();
	if (!Controller)
	{
		return;
	}

	Controller->SetViewTargetWithBlend(MainViewTarget, 1.0f);

	Controller->bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	Controller->SetInputMode(InputMode);

	const UWorld* World = Controller->GetWorld();
	check(World);
	UGameViewportClient* Viewport = World->GetGameViewport();
	check(Viewport);
	Viewport->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently);
}

APlayerController* UBlendCameraComponent::GetViewingController() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	APlayerController* Controller;

	APawn* PawnOwner = Cast<APawn>(Owner);
	if (PawnOwner)
	{
		Controller = PawnOwner->GetLocalViewingPlayerController();
	}
	else
	{
		Controller = Cast<APlayerController>(Owner);
		if (!Controller)
		{
			return nullptr;
		}
	}

	return Controller;
}

