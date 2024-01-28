// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SharedComponents/InteractableComponent.h"

#include "InteractionComponent.generated.h"

class ADrJonesCharacter;
class UDrJonesWidgetBase;
class UWidgetManager;

static TAutoConsoleVariable<bool> CVarInteraction(
	TEXT("NS.Interaction"),
	false,
	TEXT("Interaction helper function"),
	ECVF_Cheat
);

UCLASS(ClassGroup = "PlayerComponents", meta = (BlueprintSpawnableComponent),
	HideCategories = (Variable, Tags, ComponentTick, Activation, AssetUserData, Replication, ComponentReplication, Cooking, Collision))
class DR_JONES_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UInteractionComponent();

	void SetupPlayerInput(UInputComponent* InputComponent);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UInteractableComponent* FetchInteractiveComponent() const;
	void UpdateInteractionUI();
	
	void Interact();
	void AltInteract();
	
	static bool IsInteractable(const AActor& ActorToCheck);
	static bool IsInteractable(const UMeshComponent& ComponentToCheck);

	static UInteractableComponent* GetAttachedInteractableComponent(const USceneComponent& ComponentToCheck);

	UFUNCTION(BlueprintPure)
	UInteractableComponent* GetSelectedInteractiveComponent() const { return SelectedInteractiveComponent; }
	
protected:
	TWeakObjectPtr<ADrJonesCharacter> Owner;
	TWeakObjectPtr<UWidgetManager> WidgetManager;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Interaction|UI")
	TSubclassOf<UDrJonesWidgetBase> InteractionUI;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UInteractableComponent> SelectedInteractiveComponent;

	UPROPERTY(Transient)
	TObjectPtr<UInteractableComponent> PreviousSelectedInteractiveActor;
};
