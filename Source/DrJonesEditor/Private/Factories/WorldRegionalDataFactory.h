﻿#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "WorldRegionalDataFactory.generated.h"

UCLASS(MinimalAPI, HideCategories = Object, CollapseCategories)
class UWorldRegionalDataFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	// TODO: Add custom editor with a map and everything

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
