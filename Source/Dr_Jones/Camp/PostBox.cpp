// Property of Night Shift Games, all rights reserved.

#include "PostBox.h"

#include "Interaction/InteractableComponent.h"

APostBox::APostBox()
{
	PostBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PostBoxMesh"));
	RootComponent = PostBoxMesh;

	InteractableComponent->SetupAttachment(PostBoxMesh);
}
