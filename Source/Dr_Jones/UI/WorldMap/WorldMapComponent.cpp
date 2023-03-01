// Property of Night Shift Games, all rights reserved.


#include "UI/WorldMap/WorldMapComponent.h"
#include "Components/WidgetComponent.h"
#include "WorldMapWidget.h"

UWorldMapComponent::UWorldMapComponent()
{
	WidgetClass = UWorldMapWidget::StaticClass();
	PrimaryComponentTick.bCanEverTick = true;
}

void UWorldMapComponent::BeginPlay()
{
	Super::BeginPlay();

	UWidgetComponent* WorldMapWidgetComponent = GetWidgetComponent();
	if (!WorldMapWidgetComponent)
	{
		return;
	}

	WorldMapWidget = Cast<UWorldMapWidget>(WorldMapWidgetComponent->GetWidget());
}

void UWorldMapComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("World Map OnComponentCreated"));
}

void UWorldMapComponent::UpdateConstruct()
{
	UWidgetComponent* WorldMapWidgetComponent = GetWidgetComponent();
	if (!WorldMapWidgetComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("World Map Widget Component has not been set."));
		return;
	}

	WorldMapWidgetComponent->SetWidgetClass(WidgetClass);
}

void UWorldMapComponent::PostInitProperties()
{
	Super::PostInitProperties();

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("World Map Component PostInitProperties"));
}

#if WITH_EDITOR

void UWorldMapComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (UWidgetComponent* WorldMapWidgetComponent = GetWidgetComponent())
	{
		WorldMapWidgetComponent->UpdateWidget();
	}
}

#endif

UWidgetComponent* UWorldMapComponent::GetWidgetComponent() const
{
	return WidgetComponent;
}

