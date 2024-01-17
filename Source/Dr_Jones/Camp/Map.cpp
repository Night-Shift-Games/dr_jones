// Property of Night Shift Games, all rights reserved.

#include "Map.h"

#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "SharedComponents/BlendCameraComponent.h"

AMap::AMap()
{
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;
	
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable Component"));

	MapWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WorldWidget"));
	MapWidgetComponent->SetupAttachment(StaticMeshComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(StaticMeshComponent);
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
	BlendCameraComponent->SetViewTarget(this, MapWidgetComponent->GetWidget());
	EnableInput(Character.GetController<APlayerController>());
}

void AMap::UnfocusOnMap()
{
	if (!BlendCameraComponent)
	{
		return;
	}
	DisableInput(BlendCameraComponent->GetOwner<ADrJonesCharacter>()->GetController<APlayerController>());
	BlendCameraComponent->ResetViewTarget();
	BlendCameraComponent = nullptr;
}

