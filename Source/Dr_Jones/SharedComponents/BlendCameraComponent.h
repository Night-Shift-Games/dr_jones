// Property of Night Shift Games, all rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "UI/DrJonesWidgetBase.h"

#include "BlendCameraComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent),
	HideCategories=(Tick))
class DR_JONES_API UBlendCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBlendCameraComponent();
	virtual void BeginPlay() override;
	
	APlayerController* GetViewingController() const;
	
	UFUNCTION(BlueprintCallable, Category = "DrJones|Camera")
	void SetViewTarget(AActor* NewViewTarget, UUserWidget* WidgetToFocus);

	UFUNCTION(BlueprintCallable, Category = "DrJones|Camera")
	void ResetViewTarget();
	
public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<AActor> MainViewTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDrJonesWidgetBase> WidgetClass;
};
