// Property of Night Shift Games, all rights reserved.

#include "BlendCameraComponent.h"

#include "Utilities.h"
#include "Blueprint/UserWidget.h"
#include "Player/DrJonesCharacter.h"
#include "Player/WidgetManager.h"

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

void UBlendCameraComponent::SetViewTarget(AActor* NewViewTarget, const float BlendTime)
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
	
	Controller->SetViewTargetWithBlend(NewViewTarget, BlendTime);
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

	Utilities::GetWidgetManager(*this).SetWidgetVisibility(WidgetClass, ESlateVisibility::Visible);
	
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

