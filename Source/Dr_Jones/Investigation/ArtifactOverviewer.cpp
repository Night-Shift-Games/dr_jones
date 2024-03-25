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
	ArtifactToOverview->SetActorRotation(CameraForwardVector.ToOrientationRotator());
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
	const FVector2D InputVector = InputActionValue.Get<FVector2D>();
	FVector2D OutDirection;
	float OutLength;
	InputVector.ToDirectionAndLength(OutDirection, OutLength);
	OutDirection.Normalize();
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, OutDirection.ToString());
	FVector Direction = FVector(OutDirection.X, OutDirection.Y, 0.0);

	RequestRotate(Direction);
}

void UArtifactOverviewer::SetupInputComponent(UInputComponent& InputComponent)
{
}

void UArtifactOverviewer::RequestRotate(FVector& Direction)
{
	// if (bIsRotating)
	// {
	// 	return;
	// }
	// bIsRotating = true;

	FRotator Rotation = FRotator(Direction.X, Direction.Y, Direction.Z) * 90;
	
	FTransform Trans = ArtifactToOverview->GetTransform();
	Trans.ConcatenateRotation(Rotation.Quaternion());
	ArtifactToOverview->SetActorRotation(Trans.Rotator());
	
	//TargetRotation = LocalTargetVector;
	//
	// GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, LocalRotation.ToOrientationRotator().ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, Rotation.ToString());
	// GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, LocalTargetVector.ToOrientationRotator().ToString());
	
	// GetWorld()->GetTimerManager().SetTimer(RotationTimer, FTimerDelegate::CreateWeakLambda(this, [&]
	// {
	// 	Rotate();
	// }),GetWorld()->GetDeltaSeconds(), true, 0.0);
}

void UArtifactOverviewer::Rotate()
{
	//FVector CurrentRotation = ArtifactToOverview->GetActorRotation().Vector();
	//FQuat NextRotation = FQuat::Slerp(CurrentRotation.ToOrientationQuat(), TargetRotation, 1.0);
	ArtifactToOverview->SetActorRotation(TargetRotation.ToOrientationQuat());
	//if ((CurrentRotation - TargetRotation.Rotator()).IsNearlyZero())
	bIsRotating = false;
}
