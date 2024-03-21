// Property of Night Shift Games, all rights reserved.

#include "ArtifactOverviewer.h"

#include "Camera/CameraComponent.h"
#include "Items/Artifacts/Artifact.h"
#include "Utilities/Utilities.h"

void UArtifactOverviewer::InitializeOverviewer(UCameraComponent* Camera, AArtifact* Artifact)
{
	CameraComponent = Camera;
	ArtifactToOverview = Artifact;
}

void UArtifactOverviewer::StartOverview()
{
	ActorTransformBeforeOverview = ArtifactToOverview->GetActorTransform();
	PreviousActorOwner = ArtifactToOverview->GetOwner();
	PreviousAttachmentSocket = ArtifactToOverview->GetAttachParentSocketName();
	
	const FVector CameraLocation = CameraComponent->GetComponentLocation();
	const FVector CameraForwardVector = CameraComponent->GetForwardVector();
	ArtifactToOverview->SetActorLocation(CameraLocation + CameraForwardVector * 50.0);
	ArtifactToOverview->AttachToComponent(CameraComponent, FAttachmentTransformRules::KeepWorldTransform);
	ArtifactToOverview->SetInteractionEnabled(false);
}

void UArtifactOverviewer::EndOverview()
{
	ArtifactToOverview->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	if (PreviousActorOwner)
	{
		ArtifactToOverview->AttachToActor(PreviousActorOwner, FAttachmentTransformRules::KeepWorldTransform, PreviousAttachmentSocket);
	}
	ArtifactToOverview->SetActorTransform(ActorTransformBeforeOverview);
	ArtifactToOverview->SetInteractionEnabled(true);
}

void UArtifactOverviewer::ApplyControl(const FInputActionValue& InputActionValue)
{
	const FVector2D ControlRotationDelta = InputActionValue.Get<FVector2D>();
	const FRotator DeltaRotation = FRotator(ControlRotationDelta.Y, ControlRotationDelta.X,0.0);
	const ADrJonesCharacter* DrJonesPawn = &Utilities::GetPlayerCharacter(*this);
	FRotator ViewRotation = DrJonesPawn->GetViewRotation();
	ViewRotation.Add(DeltaRotation.Pitch, DeltaRotation.Yaw, DeltaRotation.Roll);
	
	if (ArtifactToOverview)
	{
		ArtifactToOverview->SetActorRotation(ViewRotation);
	}
}

void UArtifactOverviewer::SetupInputComponent(UInputComponent& InputComponent)
{
}
