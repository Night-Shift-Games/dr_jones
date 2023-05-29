// Property of Night Shift Games, all rights reserved.

#include "WorldSpatialDataFactory.h"

#include "World/WorldSpatialData.h"

UWorldSpatialDataFactory::UWorldSpatialDataFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UWorldSpatialData::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UWorldSpatialDataFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UWorldSpatialData>(InParent, Class, Name, Flags);
}
