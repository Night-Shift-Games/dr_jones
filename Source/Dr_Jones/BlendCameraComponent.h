// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlendCameraComponent.generated.h"


UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DR_JONES_API UBlendCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBlendCameraComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetViewTarget(AActor* NewViewTarget, UUserWidget* WidgetToFocus);

	UFUNCTION(BlueprintCallable)
	void ResetViewTarget();

	APlayerController* GetViewingController() const;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<AActor> MainViewTarget = nullptr;
};
