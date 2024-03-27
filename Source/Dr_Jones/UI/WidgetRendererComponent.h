// Property of Night Shift Games, all rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "WidgetRendererComponent.generated.h"

class SVirtualWindow;
class FHittestGrid;
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
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintCallable)
	void RenderWidget(UWidget* Widget);

	UFUNCTION(BlueprintCallable)
	void RenderInternalWidget();

	UFUNCTION(BlueprintPure)
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

	UFUNCTION(BlueprintCallable)
	void SetInternalWidget(TSubclassOf<UUserWidget> WidgetClass);

	UFUNCTION(BlueprintPure)
	UUserWidget* GetInternalWidget() const;

protected:
	void ReleaseResources();

	void CreateInternalWidget();

public:
	// Render target size
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D DrawSize = FVector2D(2048, 1024);

	// Size of the window the widget will be drawn in
	// This can be calculated using a target surface size * PPU
	// PPU: Pixels Per (Unreal) Unit - the actual desired resolution of the widget before rendering
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D WindowSize = FVector2D(800, 600);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> InternalWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRenderOnTick = true;

protected:
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> InternalWidget;
	TSharedPtr<SWidget> InternalSlateWidget;

	FWidgetRenderer* WidgetRenderer;
	TSharedPtr<SVirtualWindow> VirtualWindow;
};
