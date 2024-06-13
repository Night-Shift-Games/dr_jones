// Property of Night Shift Games, all rights reserved.

#include "ArtifactOverviewer.h"

#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Items/Artifacts/Artifact.h"
#include "Slate/SceneViewport.h"
#include "Utilities/Utilities.h"

void UArtifactOverviewer::InitializeOverviewer(ADrJonesCharacter& PlayerPawn, UCameraComponent& Camera, AArtifact& Artifact, TFunctionRef<void(AArtifact&)> CallbackFunc)
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
	bIsOverviewing = true;

	FixCameraPenetration();
}

void UArtifactOverviewer::EndOverview()
{
	RestoreActorToPreOverviewState();
	RemoveOverviewMappingContext();
	bIsOverviewing = false;
}

void UArtifactOverviewer::FixCameraPenetration() const
{
	// Reset the artifact position for the bounds check to yield the correct results
	ArtifactToOverview->SetActorLocation(ArtifactTransformAtOverviewBeginning.GetLocation());

	FBox Box(ForceInit);
	const FTransform WorldToCamera = CameraTransformAtOverviewBeginning.Inverse();

	ArtifactToOverview->ForEachComponent<UPrimitiveComponent>(false, [&](const UPrimitiveComponent* InPrimComp)
	{
		if (InPrimComp->IsRegistered())
		{
			const FTransform ComponentToCameraSpace = InPrimComp->GetComponentTransform() * WorldToCamera;
			Box += InPrimComp->CalcBounds(ComponentToCameraSpace).GetBox();
		}
	});

	// A so-called camera "safe zone"
	FVector CameraBoxExtent = FVector(50, 100, 100);
	FBox CameraBox(-CameraBoxExtent, CameraBoxExtent);
	FBox OverlapBox = CameraBox.Overlap(Box);
	const double Penetration = OverlapBox.GetSize().X;

	FVector OffsetVector = CameraTransformAtOverviewBeginning.TransformVector(FVector(Penetration, 0, 0));
	ArtifactToOverview->SetActorLocation(ArtifactTransformAtOverviewBeginning.GetLocation() + OffsetVector);
}

void UArtifactOverviewer::ApplyControl(const FInputActionValue& InputActionValue)
{
	check(ArtifactToOverview);

	const FVector2D InputVector = InputActionValue.Get<FVector2D>();
	FVector2D OutDirection;
	float OutLength;
	InputVector.ToDirectionAndLength(OutDirection, OutLength);
	OutDirection.Normalize();
	FVector Direction = FVector(OutDirection.X, -OutDirection.Y, 0.0);

	RequestRotate(Direction);
	FixCameraPenetration();
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
	static constexpr float DistanceToArtifact = 50.0f;
	ArtifactToOverview->SetActorLocation(CameraLocation + CameraForwardVector * DistanceToArtifact);
	ArtifactToOverview->SetActorRotation(CameraForwardVector.ToOrientationRotator());
	ArtifactToOverview->SetInteractionEnabled(false);

	// TODO: Check that pls
	const float AspectRatio = Viewer->GetController<APlayerController>()->GetLocalPlayer()->ViewportClient->GetGameViewport()->GetDesiredAspectRatio();
	const float VerticalFov = CameraComponent->FieldOfView / AspectRatio;
	const double DesiredVerticalSize = FMath::Tan(FMath::DegreesToRadians(VerticalFov / 2.0f)) * DistanceToArtifact;
	FVector Origin;
	FVector Extent;
	ArtifactToOverview->GetActorBounds(false, Origin, Extent);
	const double AvgAxisSize = (Extent.X + Extent.Y + Extent.Z) * 2.0 / 3.0;
	const double DesiredScale = DesiredVerticalSize / AvgAxisSize;
	ArtifactToOverview->SetActorRelativeScale3D(FVector(DesiredScale));

	CameraTransformAtOverviewBeginning = CameraComponent->GetComponentTransform();
	ArtifactTransformAtOverviewBeginning = ArtifactToOverview->GetActorTransform();
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
