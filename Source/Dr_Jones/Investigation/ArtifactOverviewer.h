// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
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
	void InitializeOverviewer(UCameraComponent* Camera, AArtifact* Artifact);
	void StartOverview();
	void EndOverview();

	UFUNCTION(BlueprintCallable)
	void ApplyControl(const FInputActionValue& InputActionValue);

	void SetupInputComponent(UInputComponent& InputComponent);

	void RequestRotate(FVector& Direction);

	void Rotate();
	
public:
	FQuat StartingRotation = FQuat::Identity;
	FVector TargetRotation = FVector::ZeroVector;

	bool bIsRotating = false;

	FTimerHandle RotationTimer;
	FTimerHandle RotationTime;
	
	UPROPERTY()
	AActor* PreviousActorOwner;
	
	FName PreviousAttachmentSocket;
	FTransform ActorTransformBeforeOverview;

	UPROPERTY()
	TObjectPtr<AArtifact> ArtifactToOverview;
	
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraComponent;
	
	TSubclassOf<UDrJonesWidgetBase> OverviewUI;
};
