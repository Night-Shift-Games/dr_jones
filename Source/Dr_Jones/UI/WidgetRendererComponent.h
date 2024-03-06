// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "WidgetRendererComponent.generated.h"

class FWidgetRenderer;
class UWidget;

UCLASS(ClassGroup = (DrJones), meta = (BlueprintSpawnableComponent))
class DR_JONES_API UWidgetRendererComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWidgetRendererComponent();
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void RenderWidget(UWidget* Widget);

	UFUNCTION(BlueprintPure)
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

protected:
	void ReleaseResources();

	void CreateAndRenderWidget();

public:
	// TODO: Fix scaling, as it should probably be absolute - it needs to take the widget desired size into consideration
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D DrawSize = FVector2D(2048, 1024);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> UMGWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRenderOnTick = true;

protected:
	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> UMGWidget;

	FWidgetRenderer* WidgetRenderer;
};
