// Property of Night Shift Games, all rights reserved.

#include "DrJonesCharacter.h"

#include "Animation/CharacterAnimationComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerComponents/HotBarComponent.h"
#include "PlayerComponents/InteractionComponent.h"
#include "PlayerComponents/ReactionComponent.h"

ADrJonesCharacter::ADrJonesCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	ReactionComponent = CreateDefaultSubobject<UReactionComponent>(TEXT("ReactionComponent"));
	HotBarComponent = CreateDefaultSubobject<UHotBarComponent>(TEXT("ToolbarComponent"));
	CharacterAnimationComponent = CreateDefaultSubobject<UCharacterAnimationComponent>(TEXT("CharacterAnimationComponent"));
}

void ADrJonesCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(RunTimeUIClass))
	{
		UUserWidget* UI = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), RunTimeUIClass, TEXT("Runtime"));

		if (IsValid(UI))
		{
			UI->AddToViewport();
		}
	}
}

void ADrJonesCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADrJonesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ADrJonesCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADrJonesCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ADrJonesCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ADrJonesCharacter::LookUp);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADrJonesCharacter::Interact);
	PlayerInputComponent->BindAxis("Scroll", this, &ADrJonesCharacter::SwitchItem);
	ReactionComponent->SetupPlayerInput(PlayerInputComponent);

	PlayerInputComponent->BindAction("PrimaryItemAction", IE_Pressed, this, &ADrJonesCharacter::PlayItemMontage);
}

void ADrJonesCharacter::DrawInteractionDebugInfo(FVector WorldLocation, FVector LineEnd, FHitResult Hit)
{
	if (CVarInteraction.GetValueOnAnyThread() != 0)
	{
		DrawDebugLine(GWorld, WorldLocation, LineEnd, FColor::Red, false, 1.f / 60, 0, 1);
		FString Debug = FString(TEXT("Pointed Actor: "));
		if (AActor* Actor = Hit.GetActor())
		{
			Debug += Actor->GetName();
		}
		GEngine->AddOnScreenDebugMessage(420, 10, FColor::Green, Debug);
	}
}

/*static*/ FHitResult ADrJonesCharacter::GetPlayerLookingAt(const float Reach)
{
	const APlayerController* Controller = UGameplayStatics::GetPlayerController(GWorld, 0);

	int32 ViewportX, ViewportY;
	Controller->GetViewportSize(ViewportX, ViewportY);
	FVector2D ScreenCenter = FVector2D(ViewportX / 2, ViewportY / 2);

	FVector WorldLocation, WorldDirection;
	if (!Controller->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
	{
		return {};
	}

	FVector LineEnd = WorldLocation + WorldDirection * Reach;
	FHitResult Hit;

	GWorld->LineTraceSingleByChannel(Hit, WorldLocation, LineEnd, ECollisionChannel::ECC_Visibility);

	DrawInteractionDebugInfo(WorldLocation, LineEnd, Hit);
	
	return Hit;
}

void ADrJonesCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector(), AxisValue, false);
}

void ADrJonesCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector(), AxisValue, false);
}

void ADrJonesCharacter::Turn(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void ADrJonesCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void ADrJonesCharacter::Interact()
{
	checkf(InteractionComponent, TEXT("InteractionComponent is missing!"));
	InteractionComponent->Interact();
}

void ADrJonesCharacter::SwitchItem(float AxisValue)
{
	if (AxisValue == 0)
	{
		return;
	}
	HotBarComponent->ChangeActiveItem(AxisValue);
}

void ADrJonesCharacter::PlayItemMontage()
{
	checkf(CharacterAnimationComponent, TEXT("Animation component is null."));

	if (const AItem* ActiveItem = HotBarComponent->GetActiveTool())
	{
		// @TODO: Unhardcode "Dig"
		CharacterAnimationComponent->DispatchItemAction(ActiveItem->GetClass(), "Dig");
	}
}

void ADrJonesCharacter::ShowInteractionUI()
{
	if (InteractionWidget && !InteractionWidget->IsInViewport())
	{
		InteractionWidget->AddToViewport();
	}
	if (InteractionWidgetUIClass && !InteractionWidget)
	{
		InteractionWidget = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), InteractionWidgetUIClass, MakeUniqueObjectName(GetController(), UUserWidget::StaticClass()));

		if (IsValid(InteractionWidget))
		{
			InteractionWidget->AddToViewport();
		}
	}
}

void ADrJonesCharacter::HideInteractionUI()
{
	if (InteractionWidget && InteractionWidget->IsInViewport())
	{
		InteractionWidget->RemoveFromViewport();
	}
}
