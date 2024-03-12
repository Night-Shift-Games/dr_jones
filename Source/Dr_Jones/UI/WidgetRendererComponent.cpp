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
	Super::OnRegister();

	if (!WidgetRenderer)
	{
		WidgetRenderer = new FWidgetRenderer;
	}
	if (!RenderTarget)
	{
		RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, TF_Default, false);
	}

	CreateInternalWidget();
	RenderInternalWidget();
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

	CreateInternalWidget();
	RenderInternalWidget();
}

void UWidgetRendererComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bRenderOnTick)
	{
		RenderInternalWidget();
	}
}

void UWidgetRendererComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName MemberPropertyName = PropertyChangedEvent.GetMemberPropertyName();
	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UWidgetRendererComponent, DrawSize))
	{
		RenderInternalWidget();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UWidgetRendererComponent::RenderWidget(UWidget* Widget)
{
	if (!FApp::CanEverRender() || !FSlateApplication::IsInitialized())
	{
		return;
	}

	if (!Widget || !WidgetRenderer)
	{
		return;
	}

	if (DrawSize.X <= 0 || DrawSize.Y <= 0)
	{
		return;
	}

	if (!VirtualWindow)
	{
		VirtualWindow = SNew(SVirtualWindow);
	}

	if (!RenderTarget)
	{
		RenderTarget = FWidgetRenderer::CreateTargetFor(DrawSize, TF_Default, false);
	}
	if (RenderTarget->SizeX != DrawSize.X || RenderTarget->SizeY != DrawSize.Y)
	{
		DrawSize.X = FMath::Max(DrawSize.X, 1);
		DrawSize.Y = FMath::Max(DrawSize.Y, 1);
		RenderTarget->ResizeTarget(DrawSize.X, DrawSize.Y);
	}
	check(IsValid(RenderTarget));

	const TSharedRef<SWidget> SlateWidget = Widget->TakeWidget();
	const TSharedPtr<SWidget> ParentWidget = SlateWidget->GetParentWidget();

	VirtualWindow->SetContent(SlateWidget);
	VirtualWindow->Resize(WindowSize);

	const FScale2D Scale(DrawSize / WindowSize);
	const FGeometry WindowGeometryRoot = FGeometry::MakeRoot(WindowSize, FSlateLayoutTransform());
	const FGeometry WindowGeometry = WindowGeometryRoot.MakeChild(WindowSize, FSlateLayoutTransform(), FSlateRenderTransform(Scale), FVector2D());
	// TODO: For some reason bDeferRenderTargetUpdate=true kills unreal
	WidgetRenderer->DrawWindow(RenderTarget->GameThread_GetRenderTargetResource(),
		VirtualWindow->GetHittestGrid(),
		VirtualWindow.ToSharedRef(),
		WindowGeometry,
		WindowGeometry.GetRenderBoundingRect(),
		FApp::GetDeltaTime(),
		false);

	if (Widget != InternalWidget)
	{
		SlateWidget->AssignParentWidget(ParentWidget);
		VirtualWindow->SetContent(InternalSlateWidget.IsValid() ? InternalSlateWidget.ToSharedRef() : SNullWidget::NullWidget);
	}
}

void UWidgetRendererComponent::RenderInternalWidget()
{
	if (InternalWidget)
	{
		RenderWidget(InternalWidget);
	}
}

void UWidgetRendererComponent::SetInternalWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	InternalWidgetClass = WidgetClass;
	CreateInternalWidget();
	if (VirtualWindow && InternalSlateWidget.IsValid())
	{
		VirtualWindow->SetContent(InternalSlateWidget.ToSharedRef());
	}
}

UUserWidget* UWidgetRendererComponent::GetInternalWidget() const
{
	return InternalWidget;
}

void UWidgetRendererComponent::ReleaseResources()
{
	if (WidgetRenderer)
	{
		BeginCleanup(WidgetRenderer);
		WidgetRenderer = nullptr;
	}
}

void UWidgetRendererComponent::CreateInternalWidget()
{
	if (InternalWidget)
	{
		check(InternalSlateWidget);
		InternalSlateWidget->AssignParentWidget(nullptr);
		if (VirtualWindow.IsValid())
		{
			VirtualWindow->SetContent(SNullWidget::NullWidget);
		}

		InternalWidget = nullptr;
		InternalSlateWidget = nullptr;
	}

	if (!InternalWidgetClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	InternalWidget = CreateWidget(World, InternalWidgetClass);
	InternalSlateWidget = InternalWidget->TakeWidget();
}
