// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "UI/DrJonesWidgetBase.h"
#include "UObject/Object.h"

#include "ArtifactOverviewer.generated.h"

class UCameraComponent;
class AArtifact;

UCLASS()
class DR_JONES_API UArtifactOverviewer : public UObject
{
	GENERATED_BODY()

public:
	void InitializeOverviewer(ADrJonesCharacter& PlayerPawn, UCameraComponent& Camera, AArtifact& Artifact, TFunctionRef<void(AArtifact&)> CallbackFunc = [](AArtifact&){});
	void StartOverview();

	UFUNCTION(BlueprintCallable)
	void EndOverview();

	UFUNCTION(BlueprintCallable)
	void ApplyControl(const FInputActionValue& InputActionValue);
	void RequestRotate(FVector& Direction);

protected:
	void SetMappingContext(UInputMappingContext& InOverviewMappingContext);
	void FetchPreOverviewActorProperties();
	void SetActorOverviewProperties();
	void AddOverviewMappingContext();
	void RemoveOverviewMappingContext();
	void SetOverviewFixedAxis();
	void RestoreActorToPreOverviewState();
	
public:
	FTransform OverviewFixedTransform = FTransform::Identity;
	FName PreviousAttachmentSocket;
	FTransform ActorTransformBeforeOverview;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<AActor> PreviousActorOwner;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<AArtifact> ArtifactToOverview;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> OverviewMappingContext;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<ADrJonesCharacter> Viewer;
};
