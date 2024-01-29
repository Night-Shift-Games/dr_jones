// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CampEntity.h"
#include "SharedComponents/InteractableComponent.h"

#include "Map.generated.h"

class UCameraComponent;
class UWidgetComponent;
class UBlendCameraComponent;

UCLASS()
class DR_JONES_API AMap : public ACampEntity
{
	GENERATED_BODY()

public:
	AMap();
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnInteract(ADrJonesCharacter* Player);

	// Travel
	void TravelTo();
	
	// Get Data From Illuminati
	void UpdateWorldData();
	
	// Focus on Widget
	void FocusOnMap(ADrJonesCharacter& Character);
	
	// Back to game
	UFUNCTION(BlueprintCallable)
	void UnfocusOnMap();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> MapWidgetComponent;

	UPROPERTY(Transient)
	TObjectPtr<UBlendCameraComponent> BlendCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;
};
