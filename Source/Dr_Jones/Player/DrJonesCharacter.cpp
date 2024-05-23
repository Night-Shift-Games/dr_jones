// Property of Night Shift Games, all rights reserved.

#include "DrJonesCharacter.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Animation/CharacterAnimationComponent.h"
#include "Camera/BlendCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Equipment/EquipmentComponent.h"
#include "Interaction/InteractionComponent.h"
#include "Journal/JournalComponent.h"
#include "Reputation/ReputationComponent.h"
#include "UI/WidgetManager.h"

ADrJonesCharacter::ADrJonesCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	JournalComponent = CreateDefaultSubobject<UJournalComponent>(TEXT("JournalComponent"));
	WidgetManager = CreateDefaultSubobject<UWidgetManager>(TEXT("WidgetManager"));
	ReputationComponent = CreateDefaultSubobject<UReputationComponent>(TEXT("ReputationComponent"));
	CharacterAnimationComponent = CreateDefaultSubobject<UCharacterAnimationComponent>(TEXT("CharacterAnimationComponent"));
	BlendCameraComponent = CreateDefaultSubobject<UBlendCameraComponent>(TEXT("BlendCameraComponent"));
}

void ADrJonesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADrJonesCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADrJonesCharacter::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	
	InteractionComponent->SetupPlayerInput(EnhancedInputComponent);
	EquipmentComponent->SetupPlayerInput(EnhancedInputComponent);
	JournalComponent->SetupPlayerInputComponent(*EnhancedInputComponent);
}

void ADrJonesCharacter::StartInspect(AArtifact* ArtifactToInspect)
{
	if (!ArtifactOverviewer)
	{
		ArtifactOverviewer = NewObject<UArtifactOverviewer>(this);
	}
	if (!IsValid(ArtifactToInspect))
	{
		return;
	}
	ArtifactOverviewer->InitializeOverviewer(*this,*FindComponentByClass<UCameraComponent>(), *ArtifactToInspect);
	ArtifactOverviewer->StartOverview();

	ChangeArtifactInteractionMode<UArtifactIdentificationMode>();
}

void ADrJonesCharacter::StopInspect(AArtifact* ArtifactToInspect)
{
	if (!ArtifactOverviewer)
	{
		return;
	}

	ChangeArtifactInteractionMode(nullptr);

	ArtifactOverviewer->EndOverview();
}

void ADrJonesCharacter::ChangeArtifactInteractionMode_Object(UArtifactInteractionMode* Mode)
{
	if (CurrentArtifactInteractionMode)
	{
		CurrentArtifactInteractionMode->End(*this);
	}

	if (!ArtifactOverviewer || !ArtifactOverviewer->bIsOverviewing || !ArtifactOverviewer->ArtifactToOverview)
	{
		return;
	}

	CurrentArtifactInteractionMode = Mode;
	if (CurrentArtifactInteractionMode)
	{
		CurrentArtifactInteractionMode->Begin(*this, *ArtifactOverviewer->ArtifactToOverview);
	}
}

UArtifactInteractionMode* ADrJonesCharacter::ChangeArtifactInteractionMode(TSubclassOf<UArtifactInteractionMode> Mode)
{
	ChangeArtifactInteractionMode_Object(FindArtifactInteractionMode(Mode));
	check(!CurrentArtifactInteractionMode || CurrentArtifactInteractionMode->IsA(Mode));
	return CurrentArtifactInteractionMode;
}

UArtifactInteractionMode* ADrJonesCharacter::FindArtifactInteractionMode(TSubclassOf<UArtifactInteractionMode> ModeClass)
{
	TObjectPtr<UArtifactInteractionMode>* FoundMode = ArtifactInteractionModes.FindByPredicate([&](UArtifactInteractionMode* Mode)
	{
		return Mode->IsA(ModeClass);
	});
	return FoundMode ? *FoundMode : nullptr;
}

void ADrJonesCharacter::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D Vector = InputActionValue.Get<FVector2D>();
	if (Vector.X != 0)
	{
		AddMovementInput(GetActorForwardVector(), Vector.X);
	}
	if (Vector.Y != 0)
	{
		AddMovementInput(GetActorRightVector(), Vector.Y);
	}
}

void ADrJonesCharacter::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D Vector = InputActionValue.Get<FVector2D>();
	if (Vector.Length() == 0)
	{
		return;
	}
	AddControllerYawInput(Vector.X);
	AddControllerPitchInput(Vector.Y);
}

