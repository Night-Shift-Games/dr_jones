// Property of Night Shift Games, all rights reserved.

#include "DigSiteBorder.h"

#include "DigSite.h"
#include "Components/SphereComponent.h"

ADigSiteBorderBreakMarker::ADigSiteBorderBreakMarker()
{
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
#if WITH_EDITORONLY_DATA
	SceneComponent->bVisualizeComponent = true;
#endif
	RootComponent = SceneComponent;

	VisSphere = CreateDefaultSubobject<USphereComponent>(TEXT("VisSphere"));
	VisSphere->SetupAttachment(RootComponent);
	VisSphere->ShapeColor = FColorList::GreenYellow;
}

void ADigSiteBorderBreakMarker::PostLoad()
{
	Super::PostLoad();
	VisSphere->SetSphereRadius(Radius);
}
#if WITH_EDITOR
void ADigSiteBorderBreakMarker::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ADigSiteBorderBreakMarker, Radius))
	{
		VisSphere->SetSphereRadius(Radius);
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
UDigSiteBorder::UDigSiteBorder()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		ensureMsgf(GetOwner<ADigSite>() != nullptr, TEXT("DigSiteBorder can only be used in DigSite Actor."));
	}
}

void UDigSiteBorder::GenerateRectangularBorder(const FDigSiteRectangularBorderDesc& BorderDesc)
{
	OnGenerateRectangularBorder(BorderDesc);
}

void UDigSiteBorderImpl::GenerateRectangularBorder(const FDigSiteRectangularBorderDesc& BorderDesc)
{
	Super::GenerateRectangularBorder(BorderDesc);
}
