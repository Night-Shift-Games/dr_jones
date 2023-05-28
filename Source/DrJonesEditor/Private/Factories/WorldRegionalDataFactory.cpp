#include "WorldRegionalDataFactory.h"

#include "World/WorldRegionalData.h"

UWorldRegionalDataFactory::UWorldRegionalDataFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UWorldRegionalData::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UWorldRegionalDataFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UWorldRegionalData>(InParent, Class, Name, Flags);
}
