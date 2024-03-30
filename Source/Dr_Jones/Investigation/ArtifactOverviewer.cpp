// Property of Night Shift Games, all rights reserved.

#include "ArtifactOverviewer.h"

#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Items/Artifacts/Artifact.h"
#include "Utilities/Utilities.h"

void UArtifactOverviewer::InitializeOverviewer(ADrJonesCharacter& PlayerPawn, UCameraComponent& Camera, AArtifact& Artifact)
{
	CameraComponent = &Camera;
	ArtifactToOverview = &Artifact;
	Viewer = &PlayerPawn;
	if (PlayerPawn.OverviewMappingContext)
	{
		SetMappingContext(*PlayerPawn.OverviewMappingContext);
	}
}

void UArtifactOverviewer::StartOverview()
{
	FetchPreOverviewActorProperties();
	SetActorOverviewProperties();
	SetOverviewFixedAxis();
	AddOverviewMappingContext();
}

void UArtifactOverviewer::EndOverview()
{
	RestoreActorToPreOverviewState();
	RemoveOverviewMappingContext();
}

void UArtifactOverviewer::ApplyControl(const FInputActionValue& InputActionValue)
{
	const FVector2D InputVector = InputActionValue.Get<FVector2D>();
	FVector2D OutDirection;
	float OutLength;
	InputVector.ToDirectionAndLength(OutDirection, OutLength);
	OutDirection.Normalize();
	FVector Direction = FVector(OutDirection.X, -OutDirection.Y, 0.0);

	RequestRotate(Direction);
}

void UArtifactOverviewer::FetchPreOverviewActorProperties()
{
	ActorTransformBeforeOverview = ArtifactToOverview->GetActorTransform();
	PreviousActorOwner = ArtifactToOverview->GetOwner();
	PreviousAttachmentSocket = ArtifactToOverview->GetAttachParentSocketName();
}

void UArtifactOverviewer::SetActorOverviewProperties()
{
	const FVector CameraLocation = CameraComponent->GetComponentLocation();
	const FVector CameraForwardVector = CameraComponent->GetForwardVector();
	ArtifactToOverview->SetActorLocation(CameraLocation + CameraForwardVector * 50.0);
	ArtifactToOverview->SetActorRotation(CameraForwardVector.ToOrientationRotator());
	ArtifactToOverview->SetInteractionEnabled(false);
}

void UArtifactOverviewer::AddOverviewMappingContext()
{
	Viewer->GetLocalViewingPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(OverviewMappingContext.Get(), 420);
}

void UArtifactOverviewer::RemoveOverviewMappingContext()
{
	Viewer->GetLocalViewingPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->RemoveMappingContext(OverviewMappingContext.Get());
}

void UArtifactOverviewer::SetOverviewFixedAxis()
{
	OverviewFixedTransform = CameraComponent->GetComponentTransform();
}

void UArtifactOverviewer::SetMappingContext(UInputMappingContext& InOverviewMappingContext)
{
	OverviewMappingContext = &InOverviewMappingContext;
}

void UArtifactOverviewer::RestoreActorToPreOverviewState()
{
	ArtifactToOverview->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	if (PreviousActorOwner)
	{
		ArtifactToOverview->AttachToActor(PreviousActorOwner, FAttachmentTransformRules::KeepWorldTransform, PreviousAttachmentSocket);
	}
	ArtifactToOverview->SetActorTransform(ActorTransformBeforeOverview);
	ArtifactToOverview->SetInteractionEnabled(true);
}

void UArtifactOverviewer::RequestRotate(FVector& Direction)
{	
	const FVector UpVector = OverviewFixedTransform.GetUnitAxis( EAxis::Z );
	const FVector RightVector = OverviewFixedTransform.GetUnitAxis( EAxis::Y );
		
	const FQuat UpQuat = FQuat(UpVector, FMath::DegreesToRadians(Direction.Y));
	const FQuat RightQuat = FQuat(RightVector, FMath::DegreesToRadians(Direction.X));

	const FRotator Rotation = (UpQuat * RightQuat).Rotator();
	
	ArtifactToOverview->AddActorWorldRotation(Rotation);
}

void UArtifactOverviewer::StartPrimaryAction()
{
	// TODO: we need some overview modes that would make it do different things to the artifact

	if (!ensure(ArtifactToOverview && Viewer))
	{
		return;
	}

	if (!ArtifactToOverview->IsDynamic())
	{
		return;
	}

	FHitResult HitResult = Utilities::GetPlayerSightTarget(300.0f, *this);
	if (HitResult.IsValidBlockingHit())
	{
		const FVector LocalPosition = ArtifactToOverview->GetActorTransform().InverseTransformPosition(HitResult.Location);
		ArtifactToOverview->VertexPaint(LocalPosition, FColor(0));
	}
}

void UArtifactOverviewer::StopPrimaryAction()
{
	if (!ensure(ArtifactToOverview))
	{
		return;
	}
}
