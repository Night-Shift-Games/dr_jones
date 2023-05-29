#include "WorldRegionalDataFactory.h"

#include "World/WorldData.h"

UWorldRegionalDataFactory::UWorldRegionalDataFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UWorldData::StaticClass();
	bCreateNew = true;
	bEditAfterNew = false;
}

UObject* UWorldRegionalDataFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UWorldData>(InParent, Class, Name, Flags);
}
