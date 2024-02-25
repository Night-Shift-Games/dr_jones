// Property of Night Shift Games, all rights reserved.

#include "Map.h"

#include "Camera/BlendCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Interaction/InteractableComponent.h"

AMap::AMap() : Super()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	MapWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WorldWidget"));
	MapWidgetComponent->SetupAttachment(StaticMeshComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(StaticMeshComponent);

	InteractableComponent->SetupAttachment(StaticMeshComponent);
}

void AMap::BeginPlay()
{
	Super::BeginPlay();
	
	InteractableComponent->InteractDelegate.AddDynamic(this, &AMap::OnInteract);
}

void AMap::OnInteract(ADrJonesCharacter* Player)
{
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is missing in Map interaction."));
		return;
	}
	
	FocusOnMap(*Player);
}

void AMap::TravelTo()
{
	unimplemented();
}

void AMap::UpdateWorldData()
{
	unimplemented();
}

void AMap::FocusOnMap(ADrJonesCharacter& Character)
{
	BlendCameraComponent = Character.GetCameraBlend();
	if (!BlendCameraComponent)
	{
		return;
	}
	BlendCameraComponent->SetViewTarget(this, 0.3f);
}

void AMap::UnfocusOnMap()
{
	if (!BlendCameraComponent)
	{
		return;
	}
	BlendCameraComponent->ResetViewTarget();
	BlendCameraComponent = nullptr;
}

