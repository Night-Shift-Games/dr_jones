// Property of Night Shift Games, all rights reserved.


#include "InteractiveActor.h"

AInteractiveActor::AInteractiveActor()
{
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	if (Mesh)
	{
		StaticMeshComponent->SetStaticMesh(Mesh);
	}
}

void AInteractiveActor::SetStaticMesh(UStaticMesh* NewMesh)
{
	StaticMeshComponent->SetStaticMesh(NewMesh);
}

void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractiveActor::Interact(APawn* Indicator)
{

}
