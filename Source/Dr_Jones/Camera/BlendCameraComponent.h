// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "BlendCameraComponent.generated.h"

class UDrJonesWidgetBase;

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
	void SetViewTarget(AActor* NewViewTarget, const float BlendTime = 0.5f);

	UFUNCTION(BlueprintCallable, Category = "DrJones|Camera")
	void ResetViewTarget();
	
public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<AActor> MainViewTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDrJonesWidgetBase> WidgetClass;
};
