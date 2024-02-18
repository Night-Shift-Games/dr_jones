// Property of Night Shift Games, all rights reserved.

#include "DigSiteBorder.h"

#include "DigSite.h"

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
