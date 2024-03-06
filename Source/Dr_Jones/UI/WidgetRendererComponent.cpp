// Property of Night Shift Games, all rights reserved.

#include "WidgetRendererComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Slate/WidgetRenderer.h"

UWidgetRendererComponent::UWidgetRendererComponent()
	: WidgetRenderer(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UWidgetRendererComponent::OnRegister()
{
	if (!IsRegistered())
	{
		if (!WidgetRenderer)
		{
			WidgetRenderer = new FWidgetRenderer;
		}
		if (!RenderTarget)
		{
			RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, TF_Default, false);
		}
	}
	CreateAndRenderWidget();
	Super::OnRegister();
}

void UWidgetRendererComponent::OnUnregister()
{
	Super::OnUnregister();
	ReleaseResources();
}

void UWidgetRendererComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(bRenderOnTick);
	CreateAndRenderWidget();
}

void UWidgetRendererComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bRenderOnTick && UMGWidget)
	{
		RenderWidget(UMGWidget);
	}
}

void UWidgetRendererComponent::RenderWidget(UWidget* Widget)
{
	if (!FApp::CanEverRender() || !FSlateApplication::IsInitialized())
	{
		return;
	}

	if (!Widget || !RenderTarget || !WidgetRenderer)
	{
		return;
	}

	if (DrawSize.X <= 0 || DrawSize.Y <= 0)
	{
		return;
	}

	// TODO: Make a SVirtualWindow from this because the DrawWidget below will do that every time
	const TSharedRef<SWidget> SlateWidget = Widget->TakeWidget();
	// TODO: For some reason bDeferRenderTargetUpdate=true kills unreal
	WidgetRenderer->DrawWidget(RenderTarget, SlateWidget, DrawSize, FApp::GetDeltaTime(), false);
}

void UWidgetRendererComponent::ReleaseResources()
{
	if (WidgetRenderer)
	{
		BeginCleanup(WidgetRenderer);
		WidgetRenderer = nullptr;
	}
}

void UWidgetRendererComponent::CreateAndRenderWidget()
{
	if (!UMGWidgetClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UMGWidget = CreateWidget(World, UMGWidgetClass);
	if (!UMGWidget)
	{
		return;
	}

	RenderWidget(UMGWidget);
}
