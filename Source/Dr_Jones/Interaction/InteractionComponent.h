// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractionComponent.generated.h"

class UInteractableComponent;
class ADrJonesCharacter;
class UDrJonesWidgetBase;
class UInputAction;
class UWidgetManager;

inline TAutoConsoleVariable<bool> CVarInteraction(
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

	void SetupPlayerInput(UEnhancedInputComponent* InputComponent);

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
	
	UPROPERTY(EditAnywhere, Category = "DrJones|Interaction")
	float InteractionRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = "DrJones|Interaction|UI")
	TSubclassOf<UDrJonesWidgetBase> InteractionUI;

	UPROPERTY(EditAnywhere, Category = "DrJones|Interaction|UI")
	TSubclassOf<UDrJonesWidgetBase> InteractionHintUI;

	UPROPERTY(EditAnywhere, Category = "DrJones|Interaction|UI")
	TObjectPtr<UInputAction> InteractionAction;

	UPROPERTY(EditAnywhere, Category = "DrJones|Interaction|UI")
	TObjectPtr<UInputAction> AlternativeInteractionAction;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UInteractableComponent> SelectedInteractiveComponent;

	UPROPERTY(Transient)
	TObjectPtr<UInteractableComponent> PreviousSelectedInteractiveActor;
};
