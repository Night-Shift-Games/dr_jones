// Property of Night Shift Games, all rights reserved.

#include "ArtifactOverviewer.h"

#include "Camera/CameraComponent.h"
#include "Items/Artifacts/Artifact.h"

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
