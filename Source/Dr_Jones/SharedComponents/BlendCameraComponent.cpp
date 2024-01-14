// Property of Night Shift Games, all rights reserved.

#include "BlendCameraComponent.h"

#include "Blueprint/UserWidget.h"

UBlendCameraComponent::UBlendCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
	
	Controller->SetViewTargetWithBlend(NewViewTarget, 0.5f);

	if (WidgetToFocus)
	{
		Controller->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
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

	Controller->SetViewTargetWithBlend(MainViewTarget, 0.3f);

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
	const APawn* Pawn = GetOwner<APawn>();
	return Pawn ? Pawn->GetController<APlayerController>() : nullptr;
}

